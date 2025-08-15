#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "tensor.h"
#include "layers.h"
#include <vector>
#include <memory>
#include <string>

/**
 * @brief Base class for all optimizers
 */
class Optimizer {
protected:
    float learning_rate;
    std::vector<Tensor*> parameters;
    std::vector<Tensor*> gradients;
    
public:
    /**
     * @brief Constructor for optimizer
     * @param learning_rate Learning rate for parameter updates
     */
    explicit Optimizer(float learning_rate = 0.01f);
    virtual ~Optimizer() = default;
    
    /**
     * @brief Set the parameters and gradients to optimize
     * @param params Vector of parameter tensors
     * @param grads Vector of gradient tensors
     */
    void set_parameters(const std::vector<Tensor*>& params, const std::vector<Tensor*>& grads);
    
    /**
     * @brief Update parameters using the computed gradients
     */
    virtual void step() = 0;
    
    /**
     * @brief Zero out all gradients
     */
    void zero_grad();
    
    /**
     * @brief Set the learning rate
     * @param lr New learning rate
     */
    void set_learning_rate(float lr);
    
    /**
     * @brief Get the learning rate
     * @return Current learning rate
     */
    float get_learning_rate() const;
};

/**
 * @brief Stochastic Gradient Descent (SGD) optimizer
 */
class SGD : public Optimizer {
private:
    float momentum;
    std::vector<Tensor> velocity;
    
public:
    /**
     * @brief Constructor for SGD optimizer
     * @param learning_rate Learning rate
     * @param momentum Momentum coefficient (0.0 to 1.0)
     */
    SGD(float learning_rate = 0.01f, float momentum = 0.0f);
    
    void step() override;
    
    /**
     * @brief Set momentum coefficient
     * @param m Momentum value
     */
    void set_momentum(float m);
};

/**
 * @brief Adam optimizer
 */
class Adam : public Optimizer {
private:
    float beta1;
    float beta2;
    float epsilon;
    int step_count;
    std::vector<Tensor> m;  // First moment
    std::vector<Tensor> v;  // Second moment
    
public:
    /**
     * @brief Constructor for Adam optimizer
     * @param learning_rate Learning rate
     * @param beta1 First moment coefficient
     * @param beta2 Second moment coefficient
     * @param epsilon Small constant for numerical stability
     */
    Adam(float learning_rate = 0.001f, float beta1 = 0.9f, float beta2 = 0.999f, float epsilon = 1e-8f);
    
    void step() override;
    
    /**
     * @brief Set beta1 coefficient
     * @param b1 Beta1 value
     */
    void set_beta1(float b1);
    
    /**
     * @brief Set beta2 coefficient
     * @param b2 Beta2 value
     */
    void set_beta2(float b2);
};

/**
 * @brief RMSprop optimizer
 */
class RMSprop : public Optimizer {
private:
    float alpha;
    float epsilon;
    std::vector<Tensor> v;  // Moving average of squared gradients
    
public:
    /**
     * @brief Constructor for RMSprop optimizer
     * @param learning_rate Learning rate
     * @param alpha Moving average coefficient
     * @param epsilon Small constant for numerical stability
     */
    RMSprop(float learning_rate = 0.001f, float alpha = 0.99f, float epsilon = 1e-8f);
    
    void step() override;
    
    /**
     * @brief Set alpha coefficient
     * @param a Alpha value
     */
    void set_alpha(float a);
};

/**
 * @brief Factory function to create optimizers
 * @param type Optimizer type ("sgd", "adam", "rmsprop")
 * @param learning_rate Learning rate
 * @return Unique pointer to the created optimizer
 */
std::unique_ptr<Optimizer> create_optimizer(const std::string& type, float learning_rate = 0.01f);

#endif // OPTIMIZER_H
