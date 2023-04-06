
import pyopenpivcore as popc

def test_generate_cartesian_grid_with_relative_offset():
    image_size = popc.size(64, 64)
    ia_size = popc.size(32, 32)
    overlap = 0.5
    result = popc.generate_cartesian_grid_with_relative_offset(image_size, ia_size, overlap)
    assert(False)
