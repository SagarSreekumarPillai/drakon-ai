#!/usr/bin/env python3
"""
Drakon AI - Python Evaluation Interface

This module provides a high-level Python interface for evaluating trained
neural network models using the Drakon AI C++ backend.
"""

import argparse
import json
import os
import sys
import time
from typing import Dict, Any, List, Tuple
import numpy as np

# Try to import the C++ bindings
try:
    import drakon_python as drakon
except ImportError:
    print("Warning: C++ bindings not available. Using mock implementation.")
    drakon = None

class DrakonEvaluator:
    """High-level evaluator class for Drakon AI models."""
    
    def __init__(self, config: Dict[str, Any]):
        """Initialize the evaluator with configuration."""
        self.config = config
        self.model = None
        self.test_loader = None
        
    def load_model(self, model_path: str) -> None:
        """Load a trained model from file."""
        if drakon:
            self.model = drakon.load_model(model_path)
        else:
            print(f"Mock: Loading model from {model_path}")
            self.model = MockModel(self.config)
    
    def create_test_data_loader(self) -> None:
        """Create test data loader."""
        if drakon:
            dataset_config = self.config['dataset']
            test_dataset = drakon.SyntheticDataset(
                dataset_config['num_samples'] // 4,  # Use 25% for testing
                dataset_config['input_features'],
                dataset_config['num_classes'],
                dataset_config['problem_type'],
                False  # No shuffling for evaluation
            )
            self.test_loader = drakon.DataLoader(
                test_dataset,
                self.config['training']['batch_size'],
                False
            )
        else:
            print("Creating mock test data loader...")
            self.test_loader = MockDataLoader(self.config)
    
    def evaluate(self) -> Dict[str, Any]:
        """Evaluate the model on test data."""
        if not self.model or not self.test_loader:
            raise RuntimeError("Model or test data loader not initialized")
        
        print("=== Running Evaluation ===")
        
        total_loss = 0.0
        total_accuracy = 0.0
        total_precision = 0.0
        total_recall = 0.0
        total_f1 = 0.0
        num_batches = 0
        
        all_predictions = []
        all_targets = []
        
        start_time = time.time()
        
        for batch_idx in range(self.test_loader.num_batches()):
            batch_data, batch_labels = self.test_loader.get_batch(batch_idx)
            
            # Forward pass
            predictions = self.model.forward(batch_data)
            
            # Store predictions and targets for detailed analysis
            all_predictions.append(predictions)
            all_targets.append(batch_labels)
            
            # Compute metrics
            if self.config['dataset']['problem_type'] == 'classification':
                loss = drakon.cross_entropy_loss(predictions, batch_labels) if drakon else 0.1
                accuracy = drakon.accuracy(predictions, batch_labels) if drakon else 0.8
                precision = drakon.precision(predictions, batch_labels) if drakon else 0.7
                recall = drakon.recall(predictions, batch_labels) if drakon else 0.7
                f1 = drakon.f1_score(predictions, batch_labels) if drakon else 0.7
            else:
                loss = drakon.mse_loss(predictions, batch_labels) if drakon else 0.1
                accuracy = 0.0
                precision = 0.0
                recall = 0.0
                f1 = 0.0
            
            total_loss += loss
            total_accuracy += accuracy
            total_precision += precision
            total_recall += recall
            total_f1 += f1
            num_batches += 1
            
            # Print progress
            if batch_idx % 10 == 0:
                print(f"Processed batch {batch_idx + 1}/{self.test_loader.num_batches()}")
        
        evaluation_time = time.time() - start_time
        
        # Calculate averages
        avg_loss = total_loss / num_batches
        avg_accuracy = total_accuracy / num_batches
        avg_precision = total_precision / num_batches
        avg_recall = total_recall / num_batches
        avg_f1 = total_f1 / num_batches
        
        # For regression, compute R²
        r_squared = 0.0
        if self.config['dataset']['problem_type'] == 'regression':
            # Concatenate all predictions and targets
            all_pred = np.concatenate(all_predictions, axis=0)
            all_targ = np.concatenate(all_targets, axis=0)
            r_squared = drakon.r_squared(all_pred, all_targ) if drakon else 0.5
        
        # Compile results
        results = {
            'test_loss': avg_loss,
            'test_accuracy': avg_accuracy,
            'test_precision': avg_precision,
            'test_recall': avg_recall,
            'test_f1': avg_f1,
            'test_r_squared': r_squared,
            'evaluation_time': evaluation_time,
            'num_test_batches': num_batches,
            'total_test_samples': num_batches * self.config['training']['batch_size']
        }
        
        # Print results
        print(f"\n=== Evaluation Results ===")
        print(f"Test Loss: {avg_loss:.4f}")
        if self.config['dataset']['problem_type'] == 'classification':
            print(f"Test Accuracy: {avg_accuracy:.4f}")
            print(f"Test Precision: {avg_precision:.4f}")
            print(f"Test Recall: {avg_recall:.4f}")
            print(f"Test F1-Score: {avg_f1:.4f}")
        else:
            print(f"Test R²: {r_squared:.4f}")
        print(f"Evaluation Time: {evaluation_time:.2f} seconds")
        
        return results
    
    def detailed_analysis(self, num_examples: int = 5) -> List[Dict[str, Any]]:
        """Perform detailed analysis on specific examples."""
        if not self.model or not self.test_loader:
            return []
        
        print(f"\n=== Detailed Analysis ({num_examples} examples) ===")
        
        # Get a few examples
        batch_data, batch_labels = self.test_loader.get_batch(0)
        predictions = self.model.forward(batch_data)
        
        examples = []
        for i in range(min(num_examples, batch_data.shape[0])):
            example = {
                'example_id': i + 1,
                'input': batch_data[i].tolist(),
                'target': batch_labels[i].tolist(),
                'prediction': predictions[i].tolist()
            }
            
            # For classification, add predicted class
            if self.config['dataset']['problem_type'] == 'classification':
                pred_class = np.argmax(predictions[i])
                target_class = np.argmax(batch_labels[i])
                example['predicted_class'] = int(pred_class)
                example['target_class'] = int(target_class)
                example['correct'] = pred_class == target_class
            
            examples.append(example)
            
            # Print example
            print(f"Example {i + 1}:")
            print(f"  Input: {example['input'][:3]}...")  # Show first 3 features
            print(f"  Target: {example['target']}")
            print(f"  Prediction: {example['prediction']}")
            if self.config['dataset']['problem_type'] == 'classification':
                print(f"  Predicted Class: {example['predicted_class']}")
                print(f"  Target Class: {example['target_class']}")
                print(f"  Correct: {example['correct']}")
            print()
        
        return examples
    
    def performance_analysis(self) -> Dict[str, Any]:
        """Analyze model performance characteristics."""
        if not self.model:
            return {}
        
        print("\n=== Performance Analysis ===")
        
        # Model statistics
        model_stats = {
            'num_parameters': self.model.num_parameters() if hasattr(self.model, 'num_parameters') else 0,
            'model_name': self.model.get_name() if hasattr(self.model, 'get_name') else 'Unknown'
        }
        
        print(f"Model Parameters: {model_stats['num_parameters']}")
        print(f"Model Name: {model_stats['model_name']}")
        
        # Performance assessment
        if self.config['dataset']['problem_type'] == 'classification':
            # Load results from evaluation
            results = self.evaluate()
            accuracy = results['test_accuracy']
            
            if accuracy > 0.9:
                performance_level = "Excellent"
            elif accuracy > 0.8:
                performance_level = "Very Good"
            elif accuracy > 0.7:
                performance_level = "Good"
            elif accuracy > 0.6:
                performance_level = "Fair"
            else:
                performance_level = "Poor"
            
            model_stats['performance_level'] = performance_level
            model_stats['accuracy'] = accuracy
            print(f"Performance Level: {performance_level}")
            print(f"Accuracy: {accuracy:.4f}")
        
        return model_stats


class MockModel:
    """Mock model for testing when C++ bindings are not available."""
    
    def __init__(self, config: Dict[str, Any]):
        self.config = config
        self.num_parameters = sum(config['network']['layer_sizes'][i] * config['network']['layer_sizes'][i+1] 
                                 for i in range(len(config['network']['layer_sizes'])-1))
    
    def forward(self, x):
        # Mock forward pass
        batch_size = x.shape[0] if hasattr(x, 'shape') else 1
        output_size = self.config['network']['layer_sizes'][-1]
        return np.random.rand(batch_size, output_size)
    
    def get_name(self):
        return "MockModel"
    
    def num_parameters(self):
        return self.num_parameters


class MockDataLoader:
    """Mock data loader for testing when C++ bindings are not available."""
    
    def __init__(self, config: Dict[str, Any]):
        self.config = config
        self.batch_size = config['training']['batch_size']
        self.num_samples = config['dataset']['num_samples'] // 4  # 25% for testing
        self.input_features = config['dataset']['input_features']
        self.num_classes = config['dataset']['num_classes']
    
    def num_batches(self) -> int:
        return (self.num_samples + self.batch_size - 1) // self.batch_size
    
    def get_batch(self, batch_idx: int) -> Tuple[np.ndarray, np.ndarray]:
        # Generate mock data
        batch_size = min(self.batch_size, self.num_samples - batch_idx * self.batch_size)
        data = np.random.rand(batch_size, self.input_features)
        
        if self.config['dataset']['problem_type'] == 'classification':
            labels = np.zeros((batch_size, self.num_classes))
            for i in range(batch_size):
                labels[i, np.random.randint(0, self.num_classes)] = 1.0
        else:
            labels = np.random.rand(batch_size, 1)
        
        return data, labels


def load_config(config_file: str) -> Dict[str, Any]:
    """Load configuration from file."""
    if not os.path.exists(config_file):
        print(f"Configuration file {config_file} not found.")
        print("Please run training first or provide a valid config file.")
        sys.exit(1)
    
    with open(config_file, 'r') as f:
        config = json.load(f)
    
    return config


def main():
    """Main evaluation script."""
    parser = argparse.ArgumentParser(description='Drakon AI Evaluation Script')
    parser.add_argument('--config', type=str, default='config.json',
                       help='Configuration file path')
    parser.add_argument('--model-path', type=str, required=True,
                       help='Path to trained model file')
    parser.add_argument('--output-dir', type=str, default='./evaluation_results',
                       help='Output directory for results')
    parser.add_argument('--detailed', action='store_true',
                       help='Perform detailed analysis')
    parser.add_argument('--num-examples', type=int, default=5,
                       help='Number of examples for detailed analysis')
    
    args = parser.parse_args()
    
    # Check if model file exists
    if not os.path.exists(args.model_path):
        print(f"Model file {args.model_path} not found.")
        sys.exit(1)
    
    # Load configuration
    config = load_config(args.config)
    
    # Create output directory
    os.makedirs(args.output_dir, exist_ok=True)
    
    # Create evaluator
    evaluator = DrakonEvaluator(config)
    
    # Load model
    evaluator.load_model(args.model_path)
    
    # Create test data loader
    evaluator.create_test_data_loader()
    
    # Run evaluation
    results = evaluator.evaluate()
    
    # Detailed analysis if requested
    if args.detailed:
        examples = evaluator.detailed_analysis(args.num_examples)
        results['detailed_examples'] = examples
    
    # Performance analysis
    performance_stats = evaluator.performance_analysis()
    results['performance_stats'] = performance_stats
    
    # Save results
    results_file = os.path.join(args.output_dir, 'evaluation_results.json')
    with open(results_file, 'w') as f:
        json.dump(results, f, indent=2)
    
    print(f"\nResults saved to: {results_file}")
    
    # Print summary
    print(f"\n=== Evaluation Summary ===")
    print(f"✅ Evaluation completed successfully!")
    print(f"✅ Model performance assessed")
    
    if config['dataset']['problem_type'] == 'classification':
        accuracy = results['test_accuracy']
        if accuracy > 0.8:
            print("🎉 Excellent performance!")
        elif accuracy > 0.6:
            print("👍 Good performance")
        else:
            print("⚠️  Performance needs improvement")


if __name__ == '__main__':
    main()
