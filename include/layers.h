#ifndef LAYERS_H
#define LAYERS_H

#include "tensor.h"
#include "activation_functions.h"
#include <vector>
#include <memory>
#include <string>

/**
 * @brief Base class for all neural network layers
 */
class Layer {
public:
    virtual ~Layer() = default;
    
    /**
     * @brief Forward pass through the layer
     * @param input Input tensor
     * @return Output tensor
     */
    virtual Tensor forward(const Tensor& input) = 0;
    
    /**
     * @brief Backward pass through the layer
     * @param grad_output Gradient of the output
     * @return Gradient of the input
     */
    virtual Tensor backward(const Tensor& grad_output) = 0;
    
    /**
     * @brief Get the number of parameters in this layer
     * @return Number of parameters
     */
    virtual size_t num_parameters() const = 0;
    
    /**
     * @brief Get all parameters as a vector of tensors
     * @return Vector of parameter tensors
     */
    virtual std::vector<Tensor*> get_parameters() = 0;
    
    /**
     * @brief Get all parameter gradients as a vector of tensors
     * @return Vector of parameter gradient tensors
     */
    virtual std::vector<Tensor*> get_gradients() = 0;
    
    /**
     * @brief Get the layer name
     * @return Layer name
     */
    virtual std::string get_name() const = 0;
};

/**
 * @brief Linear (fully connected) layer
 */
class LinearLayer : public Layer {
private:
    Tensor weights;
    Tensor bias;
    Tensor weight_gradients;
    Tensor bias_gradients;
    Tensor last_input;
    std::string name_;
    
public:
    /**
     * @brief Constructor for linear layer
     * @param input_size Number of input features
     * @param output_size Number of output features
     * @param name Layer name
     */
    LinearLayer(size_t input_size, size_t output_size, const std::string& name = "linear");
    
    Tensor forward(const Tensor& input) override;
    Tensor backward(const Tensor& grad_output) override;
    size_t num_parameters() const override;
    std::vector<Tensor*> get_parameters() override;
    std::vector<Tensor*> get_gradients() override;
    std::string get_name() const override;
};

/**
 * @brief Multi-Layer Perceptron (MLP) - a sequence of linear layers with activations
 */
class MLP : public Layer {
private:
    std::vector<std::unique_ptr<Layer>> layers;
    std::string name_;
    
public:
    /**
     * @brief Constructor for MLP
     * @param layer_sizes Vector of layer sizes (input_size, hidden_sizes..., output_size)
     * @param activation_type Type of activation function ("relu", "sigmoid", "tanh")
     * @param name Layer name
     */
    MLP(const std::vector<size_t>& layer_sizes, const std::string& activation_type = "relu", 
        const std::string& name = "mlp");
    
    Tensor forward(const Tensor& input) override;
    Tensor backward(const Tensor& grad_output) override;
    size_t num_parameters() const override;
    std::vector<Tensor*> get_parameters() override;
    std::vector<Tensor*> get_gradients() override;
    std::string get_name() const override;
    
    /**
     * @brief Add a layer to the MLP
     * @param layer Layer to add
     */
    void add_layer(std::unique_ptr<Layer> layer);
};

/**
 * @brief Dropout layer for regularization
 */
class DropoutLayer : public Layer {
private:
    float dropout_rate;
    Tensor mask;
    bool training;
    std::string name_;
    
public:
    /**
     * @brief Constructor for dropout layer
     * @param dropout_rate Probability of dropping neurons (0.0 to 1.0)
     * @param name Layer name
     */
    DropoutLayer(float dropout_rate = 0.5f, const std::string& name = "dropout");
    
    Tensor forward(const Tensor& input) override;
    Tensor backward(const Tensor& grad_output) override;
    size_t num_parameters() const override;
    std::vector<Tensor*> get_parameters() override;
    std::vector<Tensor*> get_gradients() override;
    std::string get_name() const override;
    
    /**
     * @brief Set training mode
     * @param is_training Whether the model is in training mode
     */
    void set_training(bool is_training);
};

#endif // LAYERS_H
