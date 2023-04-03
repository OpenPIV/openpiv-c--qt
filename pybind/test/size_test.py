
import pyopenpivcore as popc

def test_default_construction():
    s = popc.size()
    assert(s.width() == 0)
    assert(s.height() == 0)

def test_non_zero():
    s = popc.size(20, 30)
    assert(s.width() == 20)
    assert(s.height() == 30)
    assert(s.area() == 600)
    assert(s.components() == [20, 30])

def test_assignment():
    s1 = popc.size(20, 30)
    s2 = s1
    assert(s2.width() == 20)
    assert(s2.height() == 30)

def test_equality():
    s1 = popc.size(20, 30)
    s2 = s1
    assert(s1 == s2)

def test_addition():
    s1 = popc.size(20, 30)
    s2 = popc.size(2, 3)
    assert(s1 + s2 == popc.size(22, 33))

def test_self_addition():
    s1 = popc.size(20, 30)
    s2 = popc.size(2, 3)
    s1 += s2
    assert(s1 == popc.size(22, 33))

def test_subtraction():
    s1 = popc.size(20, 30)
    s2 = popc.size(2, 3)
    assert(s1 - s2 == popc.size(18, 27))

def test_self_subtraction():
    s1 = popc.size(20, 30)
    s2 = popc.size(2, 3)
    s1 -= s2
    assert(s1 == popc.size(18, 27))

def test_transpose():
    s = popc.size(20, 30)
    assert(popc.transpose(s) == popc.size(30, 20))

def test_maximal_size():
    s = popc.size(20, 30)
    assert(popc.maximal_size(s) == popc.size(30, 30))

def test_minimal_size():
    s = popc.size(20, 30)
    assert(popc.minimal_size(s) == popc.size(20, 20))

def test_string():
    s = popc.size(20, 30)
    assert(str(s) == '[20,30]')
