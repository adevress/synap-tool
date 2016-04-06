#include "nrnv1reader.hpp"

NRNv1Reader::NRNv1Reader(const std::string & nrnDir) :
    _nrnDir(nrnDir),
    _nrn(nrnDir + std::string("/nrn.h5")),
    _nrn_summary(nrnDir + std::string("/nrn_summary.h5")),
    _nbNeurons(0)
{

}



size_t NRNv1Reader::getTotalNbNeurons() const{
    if(_nbNeurons == 0){
        std::vector<std::string> names = _nrn_summary.listObjectNames();
        _nbNeurons = std::count_if(names.begin(), names.end(), [](const std::string & str){
            if(str.size() > 0 && str[0] == 'a')
                return true;
            return false;
        });
    }
    return _nbNeurons;
}


boost::multi_array<double, 2> NRNv1Reader::getNrnData(size_t neuronId) const{
    boost::multi_array<double, 2> ret;
    HighFive::DataSet neuron_data = _nrn.getDataSet(std::string("/a")+ std::to_string(neuronId +1 ));
    neuron_data.read(ret);
    return ret;
}
