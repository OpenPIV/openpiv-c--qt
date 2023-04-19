
import pytest
import pyopenpivcore as popc


def test_default_construction():
    p = popc.point2_i()
    assert(p == popc.point2_i(0, 0))

def test_list_construction():
    p = popc.point2_i( [20, 30] )
    assert(p[0] == 20)
    assert(p[1] == 30)

def test_non_zero():
    p = popc.point2_i(20, 30)
    assert(p[0] == 20)
    assert(p[1] == 30)

def test_conversion():
    p_i = popc.point2_i(20, 30)
    p_d = popc.point2_d(p_i.components())
    assert(p_d == popc.point2_d(20, 30))

def test_get_item():
    p = popc.point2_i()
    assert(p[0] == 0)
    assert(p[1] == 0)
    with pytest.raises(IndexError) as e:
        p[2]

def test_set_item():
    p = popc.point2_i()
    p[0] = 10
    p[1] = 20
    assert(p == popc.point2_i(10, 20))

def test_components():
    assert( popc.point2_i(10, 20).components() == [10, 20] )
    assert( popc.point2_i(10, 20).data() == [10, 20] )

def test_equality():
    p = popc.point2_i(40, 50)
    assert(p == popc.point2_i(40, 50))
    assert(p != popc.point2_i(50, 40))

def test_inequality():
    p = popc.point2_i(10, 1)
    assert(p > popc.point2_i(5, 0))
    assert(p >= popc.point2_i(10, 1))
    assert(p < popc.point2_i(20, 1))
    assert(p < popc.point2_i(1, 10))
    assert(p <= popc.point2_i(10, 1))
