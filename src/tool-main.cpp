/*
   Copyright (C) 2016, Adrien Devresse <adrien.devresse@epfl.ch>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA

*/

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <future>
#include <atomic>
#include <system_error>

#include "nrnv1reader.hpp"

const std::string delimiter_csv = ";";

void print_help(const std::string & program_prefix){
    std::cout << "Usage: \n"
              << "\t" << program_prefix << ": [command] [options]\n"
              << "\t\t help: this help message\n"
              << "\t\t convert [nrn_directory]: convert nrn files to CSV\n"
              << "\n";
}


void steps_logging(const std::string & msg){
    static size_t line =0;
    std::cerr << ++line << ": " << msg << "\n";
}


void nrn_a_converter(const NRNv1Reader & reader, size_t nrnid, int fd_out){
    const std::string nrnid_str = std::to_string(nrnid+1);


    std::unique_ptr< boost::multi_array<double, 2> > data;
    std::string csv_neuron;

    try{
        data.reset(new boost::multi_array<double, 2>( reader.getNrnData(nrnid) ));
    }catch(std::exception & e){
        steps_logging("Impossible to Open dataset for neuron id:" + std::to_string(nrnid) + ", might have no connexion, skip...");
        return;
    }

    if(data->shape()[1] != 19){
        throw std::runtime_error("Invalid NRNv1 file format: more than 19 columns per neurons");
    }

    csv_neuron.reserve(data->shape()[0]*19*6);

    for(auto line = data->begin(); line < data->end(); ++line){
        csv_neuron += nrnid_str;
        csv_neuron += delimiter_csv;
        for(auto column = line->begin(); column < line->end(); ++column){
            csv_neuron += std::to_string(*column);
            csv_neuron += delimiter_csv;
        }
        csv_neuron += "\n";
    }

    do{
        errno =0;
        if( write(fd_out, csv_neuron.c_str(), csv_neuron.size()) < 0){
            if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN){
                continue;
            }
            throw std::system_error(std::error_code(errno, std::system_category()), "Unable to write nrndata to output file");
        }
    }while(0);

}

void nrn_slice_converter(const NRNv1Reader & reader, size_t executorId, size_t executorTotal, size_t total, int fd_out, std::atomic<size_t> & counter){
    for(size_t i =0; i < total; ++i){
        if( (i % executorTotal) == executorId){
            nrn_a_converter(reader, i, fd_out);
            counter++;
        }
    }
}



void export_to_cvs(const std::string & nrn_dir){

    int fd_out = STDOUT_FILENO;
    steps_logging(std::string("Open nrn files in directory ") + nrn_dir);
    NRNv1Reader reader(nrn_dir);

    size_t ntotal = reader.getTotalNbNeurons();
    steps_logging( std::string(" ") + std::to_string(ntotal) + " neurons found in nrn_summary.h5 ");

    size_t nparallel = std::thread::hardware_concurrency();
    steps_logging( "launch convertion with "+ std::to_string(nparallel) + " threads");

    std::atomic<size_t> counter(0);
    std::vector<std::future<void> > exec_futures;

    for(size_t i =0; i < nparallel; ++i){
         exec_futures.emplace_back( std::async( std::launch::async,
            [=, &reader, &counter]() {
            nrn_slice_converter(reader, i, nparallel, ntotal, fd_out, counter);
        }

        ));
    }

    steps_logging( "waiting for task completion ");
    for(auto & future: exec_futures){
        std::future_status status;
        do{
            steps_logging(" Done " + std::to_string(counter.load()) + "/" + std::to_string(ntotal));
            status = future.wait_for(std::chrono::seconds(2));
        }while(status != std::future_status::ready);
        future.get();
    }

}


int main(int argc, char** argv){
    if(argc != 3 || std::string(argv[1]) != "convert" ){
        print_help(argv[0]);
        exit(1);
    }

    try{
        export_to_cvs(argv[2]);
    }catch(std::exception & e){
        std::cerr << "Err: " << e.what();
        return -1;
    }
    return 0;
}


