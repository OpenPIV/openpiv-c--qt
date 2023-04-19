
import pyopenpivcore as popc

def sort_grid( grid ):
    return sorted(grid, key=lambda r: r.topLeft())


def test_margin():
    interrogation = popc.size( 32, 32 )
    generated = popc.generate_cartesian_grid_with_relative_offset( [100, 50], interrogation, 0.5 )

    expected = []
    for tl in [ [2, 1],  [18, 1],  [34, 1],  [50, 1],  [66, 1],
                [2, 17], [18, 17], [34, 17], [50, 17], [66, 17] ]:
        expected.append( popc.rect(tl, interrogation) )

    assert( len(generated) == len(expected) )
    assert( sort_grid( generated ) == sort_grid( expected ) )


def test_exact_1x1():
    interrogation = popc.size( 32, 32 )
    generated = popc.generate_cartesian_grid_with_relative_offset( [32, 32], interrogation, 0.5 )
    expected = [ popc.rect( [0, 0], interrogation ) ]

    assert( len(generated) == len(expected) )
    assert( sort_grid( generated ) == sort_grid( expected ) )


def test_exact_3x3():
    interrogation = popc.size( 32, 32 )
    generated = popc.generate_cartesian_grid_with_relative_offset( [64, 64], interrogation, 0.5 )

    expected = []
    for  tl in [ [0, 0],   [16, 0],  [32, 0],
                 [0, 16],  [16, 16], [32, 16],
                 [0, 32],  [16, 32], [32, 32] ]:
        expected.append( popc.rect( tl, interrogation ) )

    assert( len(generated) == len(expected) )
    assert( sort_grid( generated ) == sort_grid( expected ) )


def test_exact_5x5():
    interrogation = popc.size( 32, 32 )
    generated = popc.generate_cartesian_grid_with_relative_offset( [64, 64], interrogation, 0.25 )

    expected = []
    for tl in [ [0, 0],   [8, 0],   [16, 0],   [24, 0],  [32, 0],
                [0, 8],   [8, 8],   [16, 8],   [24, 8],  [32, 8],
                [0, 16],  [8, 16],  [16, 16],  [24, 16], [32, 16],
                [0, 24],  [8, 24],  [16, 24],  [24, 24], [32, 24],
                [0, 32],  [8, 32],  [16, 32],  [24, 32], [32, 32] ]:
        expected.append( popc.rect(tl, interrogation) )

    assert( len(generated) == len(expected) )
    assert( sort_grid( generated ) == sort_grid( expected ) )


def test_3x3__1_margin():
    interrogation = popc.size( 32, 32 )
    generated = popc.generate_cartesian_grid_with_relative_offset( [65, 65], interrogation, 0.5 )

    expected = []
    for tl in [ [0, 0],   [16, 0],  [32, 0],
                [0, 16],  [16, 16], [32, 16],
                [0, 32],  [16, 32], [32, 32] ]:
        expected.append( popc.rect(tl, interrogation) )

    assert( len(generated) == len(expected) )
    assert( sort_grid( generated ) == sort_grid( expected ) )


def test_3x3__2_margin():
    interrogation = popc.size( 32, 32 )
    generated = popc.generate_cartesian_grid_with_relative_offset( [66, 66], interrogation, 0.5 )

    expected = []
    for tl in [ [1, 1],   [17, 1],  [33, 1],
                [1, 17],  [17, 17], [33, 17],
                [1, 33],  [17, 33], [33, 33] ]:
        expected.append( popc.rect(tl, interrogation) )

    assert( len(generated) == len(expected) )
    assert( sort_grid( generated ) == sort_grid( expected ) )


def exact_3x3__50pc_pixel_offset():
    interrogation = popc.size( 32, 32 )
    generated = popc.generate_cartesian_grid_with_fixed_offset( [64, 64], interrogation, [16, 16] )

    expected = []
    for tl in [ [0, 0],   [16, 0],  [32, 0],
                [0, 16],  [16, 16], [32, 16],
                [0, 32],  [16, 32], [32, 32] ]:
        expected.append( popc.rect(tl, interrogation) )

    assert( len(generated) == len(expected) )
    assert( sort_grid( generated ) == sort_grid( expected ) )


def test_exact_3x5__50_25pc_pixel_offset():
    interrogation = popc.size( 32, 32 )
    generated = popc.generate_cartesian_grid_with_fixed_offset( [64, 64], interrogation, [16, 8] )

    expected = []
    for tl in [ [0, 0],   [16, 0],  [32, 0],
                [0, 8],   [16, 8],  [32, 8],
                [0, 16],  [16, 16], [32, 16],
                [0, 24],  [16, 24], [32, 24],
                [0, 32],  [16, 32], [32, 32] ]:
        expected.append( popc.rect(tl, interrogation) )

    assert( len(generated) == len(expected) )
    assert( sort_grid( generated ) == sort_grid( expected ) )


def test_exact_3x7__50_50pc_pixel_offset__nonsquare_interrogation():
    interrogation = popc.size(32, 16)
    generated = popc.generate_cartesian_grid_with_fixed_offset(
        [64, 64],      # image size
        interrogation, # interrogation size
        [16, 8]        # offset
    )

    expected = []
    for tl in [ [0, 0],   [16, 0],  [32, 0],
                [0, 8],   [16, 8],  [32, 8],
                [0, 16],  [16, 16], [32, 16],
                [0, 24],  [16, 24], [32, 24],
                [0, 32],  [16, 32], [32, 32],
                [0, 40],  [16, 40], [32, 40],
                [0, 48],  [16, 48], [32, 48] ]:
        expected.append( popc.rect(tl, interrogation) )

    assert( len(generated) == len(expected) )
    assert( sort_grid( generated ) == sort_grid( expected ) )
