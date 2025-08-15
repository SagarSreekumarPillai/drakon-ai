#!/usr/bin/env python3
"""
Drakon AI - Python Model Export Interface

This module provides utilities for exporting and importing trained models
in various formats for deployment and sharing.
"""

import argparse
import json
import os
import sys
import pickle
import numpy as np
from typing import Dict, Any, Optional
import zipfile

# Try to import the C++ bindings
try:
    import drakon_python as drakon
except ImportError:
    print("Warning: C++ bindings not available. Using mock implementation.")
    drakon = None

class ModelExporter:
    """Model export utilities for Drakon AI."""
    
    def __init__(self, config: Dict[str, Any]):
        """Initialize the exporter with configuration."""
        self.config = config
        self.model = None
        
    def load_model(self, model_path: str) -> None:
        """Load a trained model from file."""
        if drakon:
            self.model = drakon.load_model(model_path)
        else:
            print(f"Mock: Loading model from {model_path}")
            self.model = MockModel(self.config)
    
    def export_model(self, output_path: str, format_type: str = 'binary') -> None:
        """Export model in the specified format."""
        if not self.model:
            raise RuntimeError("No model loaded")
        
        print(f"Exporting model to {output_path} in {format_type} format...")
        
        if format_type == 'binary':
            self._export_binary(output_path)
        elif format_type == 'json':
            self._export_json(output_path)
        elif format_type == 'onnx':
            self._export_onnx(output_path)
        elif format_type == 'archive':
            self._export_archive(output_path)
        else:
            raise ValueError(f"Unsupported export format: {format_type}")
        
        print(f"✅ Model exported successfully to {output_path}")
    
    def _export_binary(self, output_path: str) -> None:
        """Export model in binary format."""
        if drakon:
            drakon.save_model(self.model, output_path)
        else:
            # Mock binary export
            model_data = {
                'model_type': 'MLP',
                'config': self.config,
                'parameters': self._get_mock_parameters()
            }
            with open(output_path, 'wb') as f:
                pickle.dump(model_data, f)
    
    def _export_json(self, output_path: str) -> None:
        """Export model configuration and metadata in JSON format."""
        model_info = {
            'model_type': 'MLP',
            'config': self.config,
            'model_info': {
                'num_parameters': self.model.num_parameters() if hasattr(self.model, 'num_parameters') else 0,
                'model_name': self.model.get_name() if hasattr(self.model, 'get_name') else 'Unknown'
            },
            'export_info': {
                'format': 'json',
                'version': '1.0',
                'framework': 'Drakon AI'
            }
        }
        
        with open(output_path, 'w') as f:
            json.dump(model_info, f, indent=2)
    
    def _export_onnx(self, output_path: str) -> None:
        """Export model in ONNX format (placeholder)."""
        print("ONNX export not yet implemented. Exporting as JSON instead.")
        self._export_json(output_path.replace('.onnx', '.json'))
    
    def _export_archive(self, output_path: str) -> None:
        """Export model as a complete archive with all dependencies."""
        # Create a temporary directory for the archive contents
        import tempfile
        import shutil
        
        with tempfile.TemporaryDirectory() as temp_dir:
            # Export model in binary format
            model_path = os.path.join(temp_dir, 'model.bin')
            self._export_binary(model_path)
            
            # Export configuration
            config_path = os.path.join(temp_dir, 'config.json')
            with open(config_path, 'w') as f:
                json.dump(self.config, f, indent=2)
            
            # Create README
            readme_content = f"""# Drakon AI Model Archive

This archive contains a trained neural network model exported from Drakon AI.

## Contents:
- model.bin: Trained model weights and architecture
- config.json: Model configuration and training parameters

## Usage:
1. Load the model using Drakon AI's model loading utilities
2. Use the configuration file to understand the model architecture
3. The model can be used for inference on new data

## Model Information:
- Framework: Drakon AI
- Export Version: 1.0
- Model Type: {self.config.get('network', {}).get('activation_type', 'Unknown')}
- Problem Type: {self.config.get('dataset', {}).get('problem_type', 'Unknown')}

For more information, visit: https://github.com/your-repo/drakon-ai
"""
            
            readme_path = os.path.join(temp_dir, 'README.md')
            with open(readme_path, 'w') as f:
                f.write(readme_content)
            
            # Create archive
            with zipfile.ZipFile(output_path, 'w', zipfile.ZIP_DEFLATED) as zipf:
                for root, dirs, files in os.walk(temp_dir):
                    for file in files:
                        file_path = os.path.join(root, file)
                        arc_name = os.path.relpath(file_path, temp_dir)
                        zipf.write(file_path, arc_name)
    
    def _get_mock_parameters(self) -> Dict[str, Any]:
        """Get mock parameters for export."""
        return {
            'weights': np.random.randn(100, 50).tolist(),
            'biases': np.random.randn(50).tolist(),
            'metadata': {
                'num_layers': 2,
                'activation': 'relu',
                'input_size': 100,
                'output_size': 50
            }
        }
    
    def import_model(self, import_path: str, format_type: str = 'auto') -> None:
        """Import model from the specified format."""
        print(f"Importing model from {import_path}...")
        
        if format_type == 'auto':
            format_type = self._detect_format(import_path)
        
        if format_type == 'binary':
            self._import_binary(import_path)
        elif format_type == 'json':
            self._import_json(import_path)
        elif format_type == 'archive':
            self._import_archive(import_path)
        else:
            raise ValueError(f"Unsupported import format: {format_type}")
        
        print(f"✅ Model imported successfully from {import_path}")
    
    def _detect_format(self, file_path: str) -> str:
        """Detect the format of the model file."""
        if file_path.endswith('.bin'):
            return 'binary'
        elif file_path.endswith('.json'):
            return 'json'
        elif file_path.endswith('.zip') or file_path.endswith('.tar.gz'):
            return 'archive'
        else:
            return 'binary'  # Default
    
    def _import_binary(self, import_path: str) -> None:
        """Import model from binary format."""
        if drakon:
            self.model = drakon.load_model(import_path)
        else:
            # Mock binary import
            with open(import_path, 'rb') as f:
                model_data = pickle.load(f)
            self.model = MockModel(model_data.get('config', self.config))
    
    def _import_json(self, import_path: str) -> None:
        """Import model configuration from JSON format."""
        with open(import_path, 'r') as f:
            model_info = json.load(f)
        
        # Update configuration
        if 'config' in model_info:
            self.config.update(model_info['config'])
        
        # Create mock model with imported config
        self.model = MockModel(self.config)
    
    def _import_archive(self, import_path: str) -> None:
        """Import model from archive format."""
        import tempfile
        import shutil
        
        with tempfile.TemporaryDirectory() as temp_dir:
            # Extract archive
            with zipfile.ZipFile(import_path, 'r') as zipf:
                zipf.extractall(temp_dir)
            
            # Look for model files
            model_bin_path = os.path.join(temp_dir, 'model.bin')
            config_json_path = os.path.join(temp_dir, 'config.json')
            
            if os.path.exists(model_bin_path):
                self._import_binary(model_bin_path)
            elif os.path.exists(config_json_path):
                self._import_json(config_json_path)
            else:
                raise FileNotFoundError("No model files found in archive")
    
    def get_model_info(self) -> Dict[str, Any]:
        """Get information about the loaded model."""
        if not self.model:
            return {}
        
        info = {
            'model_type': 'MLP',
            'num_parameters': self.model.num_parameters() if hasattr(self.model, 'num_parameters') else 0,
            'model_name': self.model.get_name() if hasattr(self.model, 'get_name') else 'Unknown',
            'config': self.config
        }
        
        return info
    
    def validate_model(self) -> bool:
        """Validate that the model can perform inference."""
        if not self.model:
            return False
        
        try:
            # Create a small test input
            input_size = self.config['dataset']['input_features']
            test_input = np.random.randn(1, input_size)
            
            # Perform forward pass
            output = self.model.forward(test_input)
            
            # Check output shape
            expected_output_size = self.config['dataset']['num_classes']
            if output.shape[1] == expected_output_size:
                print("✅ Model validation successful")
                return True
            else:
                print(f"❌ Model validation failed: expected output size {expected_output_size}, got {output.shape[1]}")
                return False
                
        except Exception as e:
            print(f"❌ Model validation failed with error: {e}")
            return False


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


def main():
    """Main model export script."""
    parser = argparse.ArgumentParser(description='Drakon AI Model Export Script')
    parser.add_argument('--config', type=str, default='config.json',
                       help='Configuration file path')
    parser.add_argument('--model-path', type=str, required=True,
                       help='Path to trained model file')
    parser.add_argument('--output-path', type=str, default='./exported_model',
                       help='Output path for exported model')
    parser.add_argument('--format', type=str, choices=['binary', 'json', 'onnx', 'archive'],
                       default='binary', help='Export format')
    parser.add_argument('--import-path', type=str, default='',
                       help='Path to import model from (for import mode)')
    parser.add_argument('--validate', action='store_true',
                       help='Validate model after loading')
    parser.add_argument('--info', action='store_true',
                       help='Show model information')
    
    args = parser.parse_args()
    
    # Load configuration
    if os.path.exists(args.config):
        with open(args.config, 'r') as f:
            config = json.load(f)
    else:
        print(f"Configuration file {args.config} not found.")
        sys.exit(1)
    
    # Create exporter
    exporter = ModelExporter(config)
    
    if args.import_path:
        # Import mode
        exporter.import_model(args.import_path)
    else:
        # Export mode
        if not os.path.exists(args.model_path):
            print(f"Model file {args.model_path} not found.")
            sys.exit(1)
        
        exporter.load_model(args.model_path)
    
    # Validate model if requested
    if args.validate:
        if not exporter.validate_model():
            print("❌ Model validation failed!")
            sys.exit(1)
    
    # Show model information if requested
    if args.info:
        info = exporter.get_model_info()
        print("\n=== Model Information ===")
        print(f"Model Type: {info.get('model_type', 'Unknown')}")
        print(f"Model Name: {info.get('model_name', 'Unknown')}")
        print(f"Parameters: {info.get('num_parameters', 0):,}")
        print(f"Problem Type: {config.get('dataset', {}).get('problem_type', 'Unknown')}")
        print(f"Input Features: {config.get('dataset', {}).get('input_features', 0)}")
        if config.get('dataset', {}).get('problem_type') == 'classification':
            print(f"Output Classes: {config.get('dataset', {}).get('num_classes', 0)}")
    
    # Export model if not in import mode
    if not args.import_path:
        # Determine output path with appropriate extension
        if args.format == 'binary':
            if not args.output_path.endswith('.bin'):
                args.output_path += '.bin'
        elif args.format == 'json':
            if not args.output_path.endswith('.json'):
                args.output_path += '.json'
        elif args.format == 'onnx':
            if not args.output_path.endswith('.onnx'):
                args.output_path += '.onnx'
        elif args.format == 'archive':
            if not args.output_path.endswith('.zip'):
                args.output_path += '.zip'
        
        exporter.export_model(args.output_path, args.format)
    
    print(f"\n=== Model Export/Import Complete ===")
    print(f"✅ Operation completed successfully!")


if __name__ == '__main__':
    main()
