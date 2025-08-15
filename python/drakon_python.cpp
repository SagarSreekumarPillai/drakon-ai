#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>

#include "tensor.h"
#include "layers.h"
#include "optimizer.h"
#include "dataset.h"
#include "config.h"
#include "utils.h"

namespace py = pybind11;

// Convert numpy array to Tensor
Tensor numpy_to_tensor(py::array_t<float> arr) {
    py::buffer_info buf = arr.request();
    
    if (buf.ndim != 2) {
        throw std::runtime_error("Number of dimensions must be 2");
    }
    
    std::vector<size_t> shape = {static_cast<size_t>(buf.shape[0]), 
                                 static_cast<size_t>(buf.shape[1])};
    
    Tensor tensor(shape, 0.0f);
    
    auto ptr = static_cast<float*>(buf.ptr);
    for (size_t i = 0; i < tensor.size(); ++i) {
        tensor.data()[i] = ptr[i];
    }
    
    return tensor;
}

// Convert Tensor to numpy array
py::array_t<float> tensor_to_numpy(const Tensor& tensor) {
    auto result = py::array_t<float>({tensor.rows(), tensor.cols()});
    py::buffer_info buf = result.request();
    
    auto ptr = static_cast<float*>(buf.ptr);
    for (size_t i = 0; i < tensor.size(); ++i) {
        ptr[i] = tensor.data()[i];
    }
    
    return result;
}

// Python wrapper for MLP
class PyMLP {
private:
    std::unique_ptr<MLP> mlp;
    
public:
    PyMLP(const std::vector<size_t>& layer_sizes, const std::string& activation_type, 
          const std::string& name = "python_mlp") 
        : mlp(std::make_unique<MLP>(layer_sizes, activation_type, name)) {}
    
    py::array_t<float> forward(py::array_t<float> input) {
        Tensor input_tensor = numpy_to_tensor(input);
        Tensor output_tensor = mlp->forward(input_tensor);
        return tensor_to_numpy(output_tensor);
    }
    
    py::array_t<float> backward(py::array_t<float> grad_output) {
        Tensor grad_tensor = numpy_to_tensor(grad_output);
        Tensor grad_input_tensor = mlp->backward(grad_tensor);
        return tensor_to_numpy(grad_input_tensor);
    }
    
    size_t num_parameters() const {
        return mlp->num_parameters();
    }
    
    std::string get_name() const {
        return mlp->get_name();
    }
    
    std::vector<py::array_t<float>> get_parameters() {
        auto params = mlp->get_parameters();
        std::vector<py::array_t<float>> result;
        for (auto param : params) {
            result.push_back(tensor_to_numpy(*param));
        }
        return result;
    }
    
    std::vector<py::array_t<float>> get_gradients() {
        auto grads = mlp->get_gradients();
        std::vector<py::array_t<float>> result;
        for (auto grad : grads) {
            result.push_back(tensor_to_numpy(*grad));
        }
        return result;
    }
};

// Python wrapper for Optimizer
class PyOptimizer {
private:
    std::unique_ptr<Optimizer> optimizer;
    
public:
    PyOptimizer(const std::string& type, float learning_rate) 
        : optimizer(create_optimizer(type, learning_rate)) {}
    
    void set_parameters(const std::vector<py::array_t<float>>& params, 
                       const std::vector<py::array_t<float>>& grads) {
        std::vector<Tensor*> param_tensors;
        std::vector<Tensor*> grad_tensors;
        
        for (const auto& param : params) {
            param_tensors.push_back(new Tensor(numpy_to_tensor(param)));
        }
        for (const auto& grad : grads) {
            grad_tensors.push_back(new Tensor(numpy_to_tensor(grad)));
        }
        
        optimizer->set_parameters(param_tensors, grad_tensors);
    }
    
    void step() {
        optimizer->step();
    }
    
    void zero_grad() {
        optimizer->zero_grad();
    }
    
    void set_learning_rate(float lr) {
        optimizer->set_learning_rate(lr);
    }
    
    float get_learning_rate() const {
        return optimizer->get_learning_rate();
    }
};

// Python wrapper for Dataset
class PySyntheticDataset {
private:
    std::unique_ptr<SyntheticDataset> dataset;
    
public:
    PySyntheticDataset(size_t num_samples, size_t input_features, size_t num_classes,
                       const std::string& problem_type, bool shuffle)
        : dataset(std::make_unique<SyntheticDataset>(num_samples, input_features, 
                                                    num_classes, problem_type, shuffle)) {}
    
    std::pair<py::array_t<float>, py::array_t<float>> get_batch(size_t batch_size) {
        auto [data, labels] = dataset->get_batch(batch_size);
        return {tensor_to_numpy(data), tensor_to_numpy(labels)};
    }
    
    size_t size() const {
        return dataset->size();
    }
    
    size_t input_dim() const {
        return dataset->input_dim();
    }
    
    size_t output_dim() const {
        return dataset->output_dim();
    }
    
    void reset() {
        dataset->reset();
    }
};

// Python wrapper for DataLoader
class PyDataLoader {
private:
    std::unique_ptr<DataLoader> loader;
    
public:
    PyDataLoader(const PySyntheticDataset& dataset, size_t batch_size, bool shuffle)
        : loader(std::make_unique<DataLoader>(dataset.dataset, batch_size, shuffle)) {}
    
    std::pair<py::array_t<float>, py::array_t<float>> get_batch(size_t batch_idx) {
        auto [data, labels] = loader->get_batch(batch_idx);
        return {tensor_to_numpy(data), tensor_to_numpy(labels)};
    }
    
    size_t num_batches() const {
        return loader->num_batches();
    }
    
    void reset() {
        loader->reset();
    }
};

// Loss functions
float py_mse_loss(py::array_t<float> predictions, py::array_t<float> targets) {
    Tensor pred_tensor = numpy_to_tensor(predictions);
    Tensor target_tensor = numpy_to_tensor(targets);
    return utils::loss::mse_loss(pred_tensor, target_tensor);
}

float py_cross_entropy_loss(py::array_t<float> predictions, py::array_t<float> targets) {
    Tensor pred_tensor = numpy_to_tensor(predictions);
    Tensor target_tensor = numpy_to_tensor(targets);
    return utils::loss::cross_entropy_loss(pred_tensor, target_tensor);
}

// Metrics
float py_accuracy(py::array_t<float> predictions, py::array_t<float> targets) {
    Tensor pred_tensor = numpy_to_tensor(predictions);
    Tensor target_tensor = numpy_to_tensor(targets);
    return utils::metrics::accuracy(pred_tensor, target_tensor);
}

float py_precision(py::array_t<float> predictions, py::array_t<float> targets) {
    Tensor pred_tensor = numpy_to_tensor(predictions);
    Tensor target_tensor = numpy_to_tensor(targets);
    return utils::metrics::precision(pred_tensor, target_tensor);
}

float py_recall(py::array_t<float> predictions, py::array_t<float> targets) {
    Tensor pred_tensor = numpy_to_tensor(predictions);
    Tensor target_tensor = numpy_to_tensor(targets);
    return utils::metrics::recall(pred_tensor, target_tensor);
}

float py_f1_score(py::array_t<float> predictions, py::array_t<float> targets) {
    Tensor pred_tensor = numpy_to_tensor(predictions);
    Tensor target_tensor = numpy_to_tensor(targets);
    return utils::metrics::f1_score(pred_tensor, target_tensor);
}

float py_r_squared(py::array_t<float> predictions, py::array_t<float> targets) {
    Tensor pred_tensor = numpy_to_tensor(predictions);
    Tensor target_tensor = numpy_to_tensor(targets);
    return utils::metrics::r_squared(pred_tensor, target_tensor);
}

// Model save/load functions
void save_model(const PyMLP& model, const std::string& filename) {
    // This would need to be implemented in the C++ backend
    // For now, just create a placeholder file
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        file.write("DRKON", 5);  // Magic number
        file.close();
    }
}

PyMLP load_model(const std::string& filename) {
    // This would need to be implemented in the C++ backend
    // For now, return a default model
    std::vector<size_t> layer_sizes = {10, 64, 32, 2};
    return PyMLP(layer_sizes, "relu", "loaded_model");
}

// Optimizer factory function
PyOptimizer py_create_optimizer(const std::string& type, float learning_rate) {
    return PyOptimizer(type, learning_rate);
}

PYBIND11_MODULE(drakon_python, m) {
    m.doc() = "Drakon AI Python Bindings"; // Optional module docstring
    
    // Tensor operations
    m.def("mse_loss", &py_mse_loss, "Mean squared error loss");
    m.def("cross_entropy_loss", &py_cross_entropy_loss, "Cross entropy loss");
    
    // Metrics
    m.def("accuracy", &py_accuracy, "Accuracy metric");
    m.def("precision", &py_precision, "Precision metric");
    m.def("recall", &py_recall, "Recall metric");
    m.def("f1_score", &py_f1_score, "F1 score metric");
    m.def("r_squared", &py_r_squared, "R-squared metric");
    
    // Model save/load
    m.def("save_model", &save_model, "Save model to file");
    m.def("load_model", &load_model, "Load model from file");
    
    // Optimizer factory
    m.def("create_optimizer", &py_create_optimizer, "Create optimizer");
    
    // MLP class
    py::class_<PyMLP>(m, "MLP")
        .def(py::init<const std::vector<size_t>&, const std::string&, const std::string&>(),
             py::arg("layer_sizes"), py::arg("activation_type"), py::arg("name") = "python_mlp")
        .def("forward", &PyMLP::forward, "Forward pass")
        .def("backward", &PyMLP::backward, "Backward pass")
        .def("num_parameters", &PyMLP::num_parameters, "Get number of parameters")
        .def("get_name", &PyMLP::get_name, "Get model name")
        .def("get_parameters", &PyMLP::get_parameters, "Get model parameters")
        .def("get_gradients", &PyMLP::get_gradients, "Get model gradients");
    
    // Optimizer class
    py::class_<PyOptimizer>(m, "Optimizer")
        .def(py::init<const std::string&, float>(), py::arg("type"), py::arg("learning_rate"))
        .def("set_parameters", &PyOptimizer::set_parameters, "Set parameters and gradients")
        .def("step", &PyOptimizer::step, "Take optimization step")
        .def("zero_grad", &PyOptimizer::zero_grad, "Zero gradients")
        .def("set_learning_rate", &PyOptimizer::set_learning_rate, "Set learning rate")
        .def("get_learning_rate", &PyOptimizer::get_learning_rate, "Get learning rate");
    
    // SyntheticDataset class
    py::class_<PySyntheticDataset>(m, "SyntheticDataset")
        .def(py::init<size_t, size_t, size_t, const std::string&, bool>(),
             py::arg("num_samples"), py::arg("input_features"), py::arg("num_classes"),
             py::arg("problem_type"), py::arg("shuffle"))
        .def("get_batch", &PySyntheticDataset::get_batch, "Get batch of data")
        .def("size", &PySyntheticDataset::size, "Get dataset size")
        .def("input_dim", &PySyntheticDataset::input_dim, "Get input dimension")
        .def("output_dim", &PySyntheticDataset::output_dim, "Get output dimension")
        .def("reset", &PySyntheticDataset::reset, "Reset dataset");
    
    // DataLoader class
    py::class_<PyDataLoader>(m, "DataLoader")
        .def(py::init<const PySyntheticDataset&, size_t, bool>(),
             py::arg("dataset"), py::arg("batch_size"), py::arg("shuffle"))
        .def("get_batch", &PyDataLoader::get_batch, "Get batch by index")
        .def("num_batches", &PyDataLoader::num_batches, "Get number of batches")
        .def("reset", &PyDataLoader::reset, "Reset data loader");
    
    // Utility functions
    m.def("set_seed", &utils::random::set_seed, "Set random seed");
    m.def("random_normal", [](const std::vector<size_t>& shape, float mean, float stddev) {
        Tensor tensor = utils::random::random_normal(shape, mean, stddev);
        return tensor_to_numpy(tensor);
    }, "Generate random normal tensor");
    
    m.def("random_uniform", [](const std::vector<size_t>& shape, float min, float max) {
        Tensor tensor = utils::random::random_uniform(shape, min, max);
        return tensor_to_numpy(tensor);
    }, "Generate random uniform tensor");
}
