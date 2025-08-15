#!/usr/bin/env python3
"""
Drakon AI - Python Data Preprocessing Interface

This module provides utilities for preprocessing data before training
neural networks with the Drakon AI framework.
"""

import argparse
import json
import os
import sys
import numpy as np
import pandas as pd
from typing import Dict, Any, Tuple, Optional
from sklearn.preprocessing import StandardScaler, MinMaxScaler, LabelEncoder
from sklearn.model_selection import train_test_split

class DataPreprocessor:
    """Data preprocessing utilities for Drakon AI."""
    
    def __init__(self, config: Dict[str, Any]):
        """Initialize the preprocessor with configuration."""
        self.config = config
        self.scaler = None
        self.label_encoder = None
        
    def load_csv_data(self, file_path: str) -> Tuple[np.ndarray, np.ndarray]:
        """Load data from CSV file."""
        if not os.path.exists(file_path):
            raise FileNotFoundError(f"Data file {file_path} not found")
        
        print(f"Loading data from {file_path}")
        
        # Load CSV
        df = pd.read_csv(file_path, header=0 if self.config['dataset']['has_header'] else None)
        
        # Extract features and labels
        feature_columns = self.config['dataset']['feature_columns']
        label_column = self.config['dataset']['label_column']
        
        # Handle feature columns
        if isinstance(feature_columns, list):
            X = df.iloc[:, feature_columns].values
        else:
            # Assume all columns except label column are features
            all_columns = list(range(df.shape[1]))
            all_columns.remove(label_column)
            X = df.iloc[:, all_columns].values
        
        # Handle label column
        y = df.iloc[:, label_column].values
        
        print(f"Loaded {X.shape[0]} samples with {X.shape[1]} features")
        print(f"Label shape: {y.shape}")
        
        return X, y
    
    def preprocess_features(self, X: np.ndarray) -> np.ndarray:
        """Preprocess feature data."""
        print("Preprocessing features...")
        
        # Handle missing values
        if np.isnan(X).any():
            print("Handling missing values...")
            X = self._handle_missing_values(X)
        
        # Normalize/standardize if requested
        if self.config['dataset']['normalize_data']:
            print("Normalizing data (zero mean, unit variance)...")
            self.scaler = StandardScaler()
            X = self.scaler.fit_transform(X)
        elif self.config['dataset']['standardize_data']:
            print("Standardizing data (min-max scaling)...")
            self.scaler = MinMaxScaler()
            X = self.scaler.fit_transform(X)
        
        return X
    
    def preprocess_labels(self, y: np.ndarray) -> np.ndarray:
        """Preprocess label data."""
        print("Preprocessing labels...")
        
        problem_type = self.config['dataset']['problem_type']
        
        if problem_type == 'classification':
            # Encode categorical labels
            if not np.issubdtype(y.dtype, np.number):
                print("Encoding categorical labels...")
                self.label_encoder = LabelEncoder()
                y = self.label_encoder.fit_transform(y)
            
            # Convert to one-hot encoding
            num_classes = self.config['dataset']['num_classes']
            if num_classes == 0:
                num_classes = len(np.unique(y))
                self.config['dataset']['num_classes'] = num_classes
            
            y_one_hot = np.zeros((len(y), num_classes))
            y_one_hot[np.arange(len(y)), y] = 1
            y = y_one_hot
            
            print(f"Converted to one-hot encoding with {num_classes} classes")
        
        elif problem_type == 'regression':
            # Ensure labels are float
            y = y.astype(np.float32)
            
            # Reshape if needed
            if y.ndim == 1:
                y = y.reshape(-1, 1)
        
        return y
    
    def split_data(self, X: np.ndarray, y: np.ndarray) -> Tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray]:
        """Split data into train and validation sets."""
        val_split = self.config['training']['validation_split']
        
        print(f"Splitting data: {1-val_split:.1%} train, {val_split:.1%} validation")
        
        X_train, X_val, y_train, y_val = train_test_split(
            X, y, test_size=val_split, random_state=42, shuffle=True
        )
        
        print(f"Train set: {X_train.shape[0]} samples")
        print(f"Validation set: {X_val.shape[0]} samples")
        
        return X_train, X_val, y_train, y_val
    
    def save_processed_data(self, X_train: np.ndarray, X_val: np.ndarray, 
                          y_train: np.ndarray, y_val: np.ndarray, 
                          output_dir: str) -> None:
        """Save processed data to files."""
        os.makedirs(output_dir, exist_ok=True)
        
        # Save data
        np.save(os.path.join(output_dir, 'X_train.npy'), X_train)
        np.save(os.path.join(output_dir, 'X_val.npy'), X_val)
        np.save(os.path.join(output_dir, 'y_train.npy'), y_train)
        np.save(os.path.join(output_dir, 'y_val.npy'), y_val)
        
        # Save preprocessing info
        preprocess_info = {
            'feature_scaler': self.scaler.__class__.__name__ if self.scaler else None,
            'label_encoder': self.label_encoder.__class__.__name__ if self.label_encoder else None,
            'num_features': X_train.shape[1],
            'num_classes': y_train.shape[1] if y_train.ndim > 1 else 1,
            'problem_type': self.config['dataset']['problem_type']
        }
        
        with open(os.path.join(output_dir, 'preprocess_info.json'), 'w') as f:
            json.dump(preprocess_info, f, indent=2)
        
        print(f"Processed data saved to {output_dir}")
    
    def _handle_missing_values(self, X: np.ndarray) -> np.ndarray:
        """Handle missing values in the data."""
        # Simple strategy: replace with mean for numerical, mode for categorical
        for col in range(X.shape[1]):
            col_data = X[:, col]
            if np.isnan(col_data).any():
                if np.issubdtype(col_data.dtype, np.number):
                    # Numerical column: replace with mean
                    mean_val = np.nanmean(col_data)
                    col_data[np.isnan(col_data)] = mean_val
                else:
                    # Categorical column: replace with mode
                    mode_val = pd.Series(col_data).mode()[0]
                    col_data[np.isnan(col_data)] = mode_val
                X[:, col] = col_data
        
        return X
    
    def generate_synthetic_data(self) -> Tuple[np.ndarray, np.ndarray]:
        """Generate synthetic data for testing."""
        print("Generating synthetic data...")
        
        num_samples = self.config['dataset']['num_samples']
        num_features = self.config['dataset']['input_features']
        problem_type = self.config['dataset']['problem_type']
        
        # Generate features
        X = np.random.randn(num_samples, num_features)
        
        # Generate labels based on problem type
        if problem_type == 'classification':
            num_classes = self.config['dataset']['num_classes']
            # Create a simple classification problem
            y = np.random.randint(0, num_classes, num_samples)
            y_one_hot = np.zeros((num_samples, num_classes))
            y_one_hot[np.arange(num_samples), y] = 1
            y = y_one_hot
        else:
            # Regression problem
            # Create a simple linear relationship with noise
            weights = np.random.randn(num_features)
            y = X @ weights + np.random.randn(num_samples) * 0.1
            y = y.reshape(-1, 1)
        
        print(f"Generated {num_samples} samples with {num_features} features")
        print(f"Problem type: {problem_type}")
        
        return X, y


def create_default_config() -> Dict[str, Any]:
    """Create a default configuration for preprocessing."""
    return {
        'dataset': {
            'type': 'csv',
            'file_path': '',
            'feature_columns': [],
            'label_column': 0,
            'has_header': True,
            'normalize_data': False,
            'standardize_data': True,
            'problem_type': 'classification',
            'num_classes': 0,
            'num_samples': 1000,
            'input_features': 10
        },
        'training': {
            'validation_split': 0.2,
            'shuffle_data': True
        }
    }


def main():
    """Main preprocessing script."""
    parser = argparse.ArgumentParser(description='Drakon AI Data Preprocessing Script')
    parser.add_argument('--config', type=str, default='config.json',
                       help='Configuration file path')
    parser.add_argument('--input-file', type=str, default='',
                       help='Input data file (CSV)')
    parser.add_argument('--output-dir', type=str, default='./processed_data',
                       help='Output directory for processed data')
    parser.add_argument('--synthetic', action='store_true',
                       help='Generate synthetic data instead of loading from file')
    parser.add_argument('--feature-columns', type=str, default='',
                       help='Comma-separated list of feature column indices')
    parser.add_argument('--label-column', type=int, default=0,
                       help='Label column index')
    parser.add_argument('--problem-type', type=str, choices=['classification', 'regression'],
                       default='classification', help='Problem type')
    parser.add_argument('--num-classes', type=int, default=0,
                       help='Number of classes (for classification)')
    
    args = parser.parse_args()
    
    # Load or create configuration
    if os.path.exists(args.config):
        with open(args.config, 'r') as f:
            config = json.load(f)
    else:
        config = create_default_config()
        print(f"Created default configuration: {args.config}")
    
    # Update configuration with command line arguments
    if args.input_file:
        config['dataset']['file_path'] = args.input_file
    if args.feature_columns:
        feature_cols = [int(x.strip()) for x in args.feature_columns.split(',')]
        config['dataset']['feature_columns'] = feature_cols
    if args.label_column is not None:
        config['dataset']['label_column'] = args.label_column
    if args.problem_type:
        config['dataset']['problem_type'] = args.problem_type
    if args.num_classes > 0:
        config['dataset']['num_classes'] = args.num_classes
    
    # Create preprocessor
    preprocessor = DataPreprocessor(config)
    
    # Load or generate data
    if args.synthetic or not args.input_file:
        print("Generating synthetic data...")
        X, y = preprocessor.generate_synthetic_data()
    else:
        print(f"Loading data from {args.input_file}")
        X, y = preprocessor.load_csv_data(args.input_file)
    
    # Preprocess data
    X = preprocessor.preprocess_features(X)
    y = preprocessor.preprocess_labels(y)
    
    # Split data
    X_train, X_val, y_train, y_val = preprocessor.split_data(X, y)
    
    # Save processed data
    preprocessor.save_processed_data(X_train, X_val, y_train, y_val, args.output_dir)
    
    # Update configuration with processed data info
    config['dataset']['input_features'] = X_train.shape[1]
    if config['dataset']['problem_type'] == 'classification':
        config['dataset']['num_classes'] = y_train.shape[1]
    
    # Save updated configuration
    with open(args.config, 'w') as f:
        json.dump(config, f, indent=2)
    
    print(f"\n=== Preprocessing Complete ===")
    print(f"✅ Data preprocessing completed successfully!")
    print(f"✅ Processed data saved to: {args.output_dir}")
    print(f"✅ Configuration updated: {args.config}")
    
    # Print summary
    print(f"\nData Summary:")
    print(f"  Train samples: {X_train.shape[0]}")
    print(f"  Validation samples: {X_val.shape[0]}")
    print(f"  Features: {X_train.shape[1]}")
    if config['dataset']['problem_type'] == 'classification':
        print(f"  Classes: {config['dataset']['num_classes']}")
    print(f"  Problem type: {config['dataset']['problem_type']}")


if __name__ == '__main__':
    main()
