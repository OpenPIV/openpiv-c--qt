
import pyopenpivcore as popc

def test_default_construction():
    r = popc.rect()
    assert(r.bottomLeft() == popc.point2_i(0, 0))
    assert(r.width() == 0)
    assert(r.height() == 0)
    assert(r.area() == 0)

def test_non_zero():
    r = popc.rect([40, 50], [20, 30])
    assert(r.bottomLeft()  == [40, 50])
    assert(r.bottomRight() == [60, 50])
    assert(r.topLeft()     == [40, 80])
    assert(r.topRight()    == [60, 80])

    assert(r.midpoint()    == [50, 65])

    assert(r.bottom()      == 50)
    assert(r.top()         == 80)
    assert(r.left()        == 40)
    assert(r.right()       == 60)

    assert(r.size()        == [20, 30])
    assert(r.width()       == 20)
    assert(r.height()      == 30)
    assert(r.area()        == 600)

def test_equality():
    r = popc.rect([40, 50], [20, 30])
    assert(r               == popc.rect( [40, 50], [20, 30] ))

def test_inequality():
    r = popc.rect([40, 50], [20, 30])
    assert(r               != popc.rect( [0, 50],  [20, 30] ))
    assert(r               != popc.rect( [40, 0],  [20, 30] ))
    assert(r               != popc.rect( [40, 50], [0, 30]  ))
    assert(r               != popc.rect( [40, 50], [20, 0]  ))
