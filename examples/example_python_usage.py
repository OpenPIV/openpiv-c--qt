#!/usr/bin/env python3
"""
Example: Using OpenPIV C++ from Python

This demonstrates calling the fast C++ PIV engine directly from Python
using pybind11 bindings.
"""

import openpiv_cpp
import numpy as np
import matplotlib.pyplot as plt


def main():
    print("OpenPIV C++ Python Example")
    print("=" * 50)
    
    # Process a PIV image pair
    print("\nProcessing image pair...")
    result = openpiv_cpp.piv_process(
        file1="examples/data/test2/2image_00.tif",
        file2="examples/data/test2/2image_01.tif",
        size=32,          # Interrogation window size (pixels)
        overlap=0.5       # 50% overlap
    )
    
    # Extract data
    vectors = result['vectors']  # numpy array: [x, y, u, v, sn]
    stats = result['stats']
    
    x = vectors[:, 0]
    y = vectors[:, 1]
    u = vectors[:, 2]
    v = vectors[:, 3]
    sn = vectors[:, 4]
    speed = np.sqrt(u**2 + v**2)
    
    # Print statistics
    print(f"\nResults:")
    print(f"  Image size: {result['image_size']}")
    print(f"  Vectors: {stats['vector_count']}")
    print(f"  Speed: {stats['min_speed']:.2f} - {stats['max_speed']:.2f} px (mean: {stats['mean_speed']:.2f})")
    print(f"  S/N: {stats['min_sn']:.2f} - {stats['max_sn']:.2f} (mean: {stats['mean_sn']:.2f})")
    
    # Create visualization
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))
    
    # Quiver plot
    q = ax1.quiver(x, y, u, v, speed, cmap='viridis', scale=80, width=0.003)
    ax1.set_xlabel('X (pixels)')
    ax1.set_ylabel('Y (pixels)')
    ax1.set_title(f'Velocity Field\nSpeed: {speed.min():.2f} - {speed.max():.2f} px')
    ax1.set_aspect('equal')
    ax1.invert_yaxis()
    plt.colorbar(q, ax=ax1, label='Speed (px)')
    
    # Histogram
    ax2.hist(speed, bins=50, edgecolor='black', alpha=0.7)
    ax2.axvline(stats['mean_speed'], color='r', linestyle='--', label=f"mean={stats['mean_speed']:.2f}")
    ax2.set_xlabel('Speed (pixels)')
    ax2.set_ylabel('Count')
    ax2.set_title('Speed Distribution')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('examples/data/test2/python_result.png', dpi=150)
    print(f"\nSaved plot to: examples/data/test2/python_result.png")
    
    # Compare FFT methods
    print("\nComparing FFT methods...")
    import time
    
    methods = ['complex', 'real', 'pocket', 'pocket_real']
    for method in methods:
        t0 = time.time()
        r = openpiv_cpp.cross_correlate(
            "examples/data/test2/2image_00.tif",
            "examples/data/test2/2image_01.tif",
            size=32,
            fft_type=method
        )
        elapsed = time.time() - t0
        print(f"  {method:12s}: {r['count']} vectors in {elapsed*1000:.1f} ms")
    
    plt.show()


if __name__ == '__main__':
    main()
