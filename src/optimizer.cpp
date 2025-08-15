#include "optimizer.h"
#include "utils.h"
#include <algorithm>
#include <cmath>

// Base Optimizer implementation
Optimizer::Optimizer(float learning_rate) : learning_rate(learning_rate) {}

void Optimizer::set_parameters(const std::vector<Tensor*>& params, const std::vector<Tensor*>& grads) {
    parameters = params;
    gradients = grads;
}

void Optimizer::zero_grad() {
    for (Tensor* grad : gradients) {
        for (float& val : grad->data()) {
            val = 0.0f;
        }
    }
}

void Optimizer::set_learning_rate(float lr) {
    learning_rate = lr;
}

float Optimizer::get_learning_rate() const {
    return learning_rate;
}

// SGD implementation
SGD::SGD(float learning_rate, float momentum) 
    : Optimizer(learning_rate), momentum(momentum) {}

void SGD::step() {
    if (velocity.empty()) {
        // Initialize velocity tensors
        for (Tensor* param : parameters) {
            velocity.push_back(Tensor(param->shape(), 0.0f));
        }
    }
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        Tensor* param = parameters[i];
        Tensor* grad = gradients[i];
        Tensor& vel = velocity[i];
        
        // Update velocity: v = momentum * v - learning_rate * grad
        for (size_t j = 0; j < vel.size(); ++j) {
            vel.data()[j] = momentum * vel.data()[j] - learning_rate * grad->data()[j];
        }
        
        // Update parameter: param = param + v
        for (size_t j = 0; j < param->size(); ++j) {
            param->data()[j] += vel.data()[j];
        }
    }
}

void SGD::set_momentum(float m) {
    momentum = m;
}

// Adam implementation
Adam::Adam(float learning_rate, float beta1, float beta2, float epsilon)
    : Optimizer(learning_rate), beta1(beta1), beta2(beta2), epsilon(epsilon), step_count(0) {}

void Adam::step() {
    step_count++;
    
    if (m.empty()) {
        // Initialize moment tensors
        for (Tensor* param : parameters) {
            m.push_back(Tensor(param->shape(), 0.0f));
            v.push_back(Tensor(param->shape(), 0.0f));
        }
    }
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        Tensor* param = parameters[i];
        Tensor* grad = gradients[i];
        Tensor& m_t = m[i];
        Tensor& v_t = v[i];
        
        // Update biased first moment estimate: m = beta1 * m + (1 - beta1) * grad
        for (size_t j = 0; j < m_t.size(); ++j) {
            m_t.data()[j] = beta1 * m_t.data()[j] + (1.0f - beta1) * grad->data()[j];
        }
        
        // Update biased second moment estimate: v = beta2 * v + (1 - beta2) * grad^2
        for (size_t j = 0; j < v_t.size(); ++j) {
            v_t.data()[j] = beta2 * v_t.data()[j] + (1.0f - beta2) * grad->data()[j] * grad->data()[j];
        }
        
        // Compute bias-corrected first moment estimate
        float m_hat = 1.0f / (1.0f - std::pow(beta1, step_count));
        float v_hat = 1.0f / (1.0f - std::pow(beta2, step_count));
        
        // Update parameter: param = param - learning_rate * m_hat / (sqrt(v_hat) + epsilon)
        for (size_t j = 0; j < param->size(); ++j) {
            float m_corrected = m_t.data()[j] * m_hat;
            float v_corrected = v_t.data()[j] * v_hat;
            param->data()[j] -= learning_rate * m_corrected / (std::sqrt(v_corrected) + epsilon);
        }
    }
}

void Adam::set_beta1(float b1) {
    beta1 = b1;
}

void Adam::set_beta2(float b2) {
    beta2 = b2;
}

// RMSprop implementation
RMSprop::RMSprop(float learning_rate, float alpha, float epsilon)
    : Optimizer(learning_rate), alpha(alpha), epsilon(epsilon) {}

void RMSprop::step() {
    if (v.empty()) {
        // Initialize v tensors
        for (Tensor* param : parameters) {
            v.push_back(Tensor(param->shape(), 0.0f));
        }
    }
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        Tensor* param = parameters[i];
        Tensor* grad = gradients[i];
        Tensor& v_t = v[i];
        
        // Update moving average of squared gradients: v = alpha * v + (1 - alpha) * grad^2
        for (size_t j = 0; j < v_t.size(); ++j) {
            v_t.data()[j] = alpha * v_t.data()[j] + (1.0f - alpha) * grad->data()[j] * grad->data()[j];
        }
        
        // Update parameter: param = param - learning_rate * grad / (sqrt(v) + epsilon)
        for (size_t j = 0; j < param->size(); ++j) {
            param->data()[j] -= learning_rate * grad->data()[j] / (std::sqrt(v_t.data()[j]) + epsilon);
        }
    }
}

void RMSprop::set_alpha(float a) {
    alpha = a;
}

// Factory function
std::unique_ptr<Optimizer> create_optimizer(const std::string& type, float learning_rate) {
    if (type == "sgd") {
        return std::make_unique<SGD>(learning_rate);
    } else if (type == "adam") {
        return std::make_unique<Adam>(learning_rate);
    } else if (type == "rmsprop") {
        return std::make_unique<RMSprop>(learning_rate);
    } else {
        throw std::invalid_argument("Unknown optimizer type: " + type);
    }
}
