#ifndef NRNV1READER_HPP
#define NRNV1READER_HPP


#include <string>
#include <boost/multi_array.hpp>

#include <highfive/H5File.hpp>
#include <highfive/H5Group.hpp>


class NRNv1Reader
{
public:
    NRNv1Reader(const std::string & nrnDir);

    size_t getTotalNbNeurons() const;

    /// get nrn data for a given neuron
    /// neuron id start at 0 and not 1
    boost::multi_array<double, 2> getNrnData(size_t neuronId) const;

private:
    std::string _nrnDir;
    HighFive::File _nrn, _nrn_summary;
    mutable size_t  _nbNeurons;
};


#endif // NRNV1READER_HPP
