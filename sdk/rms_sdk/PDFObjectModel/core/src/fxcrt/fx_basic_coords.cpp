#include "../../include/fxcrt/fx_ext.h"
#include <limits.h>
void FX_RECT::Normalize()
{
    if (left > right) {
        int temp = left;
        left = right;
        right = temp;
    }
    if (top > bottom) {
        int temp = top;
        top = bottom;
        bottom = temp;
    }
}
void FX_RECT::Intersect(const FX_RECT& src)
{
    FX_RECT src_n = src;
    src_n.Normalize();
    Normalize();
    left = left > src_n.left ? left : src_n.left;
    top = top > src_n.top ? top : src_n.top;
    right = right < src_n.right ? right : src_n.right;
    bottom = bottom < src_n.bottom ? bottom : src_n.bottom;
    if (left > right || top > bottom) {
        left = top = right = bottom = 0;
    }
}
void FX_RECT::Union(const FX_RECT& other_rect)
{
    Normalize();
    FX_RECT other = other_rect;
    other.Normalize();
    left = left < other.left ? left : other.left;
    right = right > other.right ? right : other.right;
    bottom = bottom > other.bottom ? bottom : other.bottom;
    top = top < other.top ? top : other.top;
}
FX_BOOL GetIntersection(FX_FLOAT low1, FX_FLOAT high1, FX_FLOAT low2, FX_FLOAT high2,
                        FX_FLOAT& interlow, FX_FLOAT& interhigh)
{
    if (low1 >= high2 || low2 >= high1) {
        return FALSE;
    }
    interlow = low1 > low2 ? low1 : low2;
    interhigh = high1 > high2 ? high2 : high1;
    return TRUE;
}
extern "C" int FXSYS_round(FX_FLOAT d)
{
    if (d <= (FX_FLOAT)INT_MIN) {
        return INT_MIN;
    }
    if (d >= (FX_FLOAT)INT_MAX) {
        return INT_MAX;
    }
    if (d >= 0.0f) {
        return (int)(d + 0.5f);
    }
    return (int)(d - 0.5f);
}
CFX_FloatRect::CFX_FloatRect(const FX_RECT& rect)
{
    left = (FX_FLOAT)(rect.left);
    right = (FX_FLOAT)(rect.right);
    bottom = (FX_FLOAT)(rect.top);
    top = (FX_FLOAT)(rect.bottom);
}
void CFX_FloatRect::Normalize()
{
    FX_FLOAT temp;
    if (left > right) {
        temp = left;
        left = right;
        right = temp;
    }
    if (bottom > top) {
        temp = top;
        top = bottom;
        bottom = temp;
    }
}
void CFX_FloatRect::Intersect(const CFX_FloatRect& other_rect)
{
    Normalize();
    CFX_FloatRect other = other_rect;
    other.Normalize();
    left = left > other.left ? left : other.left;
    right = right < other.right ? right : other.right;
    bottom = bottom > other.bottom ? bottom : other.bottom;
    top = top < other.top ? top : other.top;
    if (left > right || bottom > top) {
        left = right = bottom = top = 0;
    }
}
void CFX_FloatRect::Union(const CFX_FloatRect& other_rect)
{
    Normalize();
    CFX_FloatRect other = other_rect;
    other.Normalize();
    left = left < other.left ? left : other.left;
    right = right > other.right ? right : other.right;
    bottom = bottom < other.bottom ? bottom : other.bottom;
    top = top > other.top ? top : other.top;
}
void CFX_FloatRect::Transform(const CFX_Matrix* pMatrix)
{
    pMatrix->TransformRect(left, right, top, bottom);
}
int CFX_FloatRect::Substract4(CFX_FloatRect& s, CFX_FloatRect* pRects)
{
    Normalize();
    s.Normalize();
    int nRects = 0;
    CFX_FloatRect rects[4];
    if (left < s.left) {
        rects[nRects].left = left;
        rects[nRects].right = s.left;
        rects[nRects].bottom = bottom;
        rects[nRects].top = top;
        nRects ++;
    }
    if (s.left < right && s.top < top) {
        rects[nRects].left = s.left;
        rects[nRects].right = right;
        rects[nRects].bottom = s.top;
        rects[nRects].top = top;
        nRects ++;
    }
    if (s.top > bottom && s.right < right) {
        rects[nRects].left = s.right;
        rects[nRects].right = right;
        rects[nRects].bottom = bottom;
        rects[nRects].top = s.top;
        nRects ++;
    }
    if (s.bottom > bottom) {
        rects[nRects].left = s.left;
        rects[nRects].right = s.right;
        rects[nRects].bottom = bottom;
        rects[nRects].top = s.bottom;
        nRects ++;
    }
    if (nRects == 0) {
        return 0;
    }
    for (int i = 0; i < nRects; i ++) {
        pRects[i] = rects[i];
        pRects[i].Intersect(*this);
    }
    return nRects;
}
FX_RECT CFX_FloatRect::GetOutterRect() const
{
    CFX_FloatRect rect1 = *this;
    FX_RECT rect;
    rect.left = (int)FXSYS_floor(rect1.left);
    rect.right = (int)FXSYS_ceil(rect1.right);
    rect.top = (int)FXSYS_floor(rect1.bottom);
    rect.bottom = (int)FXSYS_ceil(rect1.top);
    rect.Normalize();
    return rect;
}
FX_RECT CFX_FloatRect::GetInnerRect() const
{
    CFX_FloatRect rect1 = *this;
    FX_RECT rect;
    rect.left = (int)FXSYS_ceil(rect1.left);
    rect.right = (int)FXSYS_floor(rect1.right);
    rect.top = (int)FXSYS_ceil(rect1.bottom);
    rect.bottom = (int)FXSYS_floor(rect1.top);
    rect.Normalize();
    return rect;
}
static void _MatchFloatRange(FX_FLOAT f1, FX_FLOAT f2, int& i1, int& i2)
{
    int length = (int)FXSYS_ceil(f2 - f1);
    int i1_1 = (int)FXSYS_floor(f1);
    int i1_2 = (int)FXSYS_ceil(f1);
    FX_FLOAT error1 = f1 - i1_1 + (FX_FLOAT)FXSYS_fabs(f2 - i1_1 - length);
    FX_FLOAT error2 = i1_2 - f1 + (FX_FLOAT)FXSYS_fabs(f2 - i1_2 - length);
    i1 = (error1 > error2) ? i1_2 : i1_1;
    i2 = i1 + length;
}
FX_RECT CFX_FloatRect::GetClosestRect() const
{
    CFX_FloatRect rect1 = *this;
    FX_RECT rect;
    _MatchFloatRange(rect1.left, rect1.right, rect.left, rect.right);
    _MatchFloatRange(rect1.bottom, rect1.top, rect.top, rect.bottom);
    rect.Normalize();
    return rect;
}
FX_BOOL CFX_FloatRect::Contains(const CFX_FloatRect& other_rect) const
{
    CFX_FloatRect n1 = *this;
    n1.Normalize();
    CFX_FloatRect n2 = other_rect;
    n2.Normalize();
    if (n2.left >= n1.left && n2.right <= n1.right && n2.bottom >= n1.bottom && n2.top <= n1.top) {
        return TRUE;
    }
    return FALSE;
}
FX_BOOL CFX_FloatRect::Contains(FX_FLOAT x, FX_FLOAT y) const
{
    CFX_FloatRect n1 = *this;
    n1.Normalize();
    return x <= n1.right && x >= n1.left && y <= n1.top && y >= n1.bottom;
}
void CFX_FloatRect::UpdateRect(FX_FLOAT x, FX_FLOAT y)
{
    if (left > x) {
        left = x;
    }
    if (right < x) {
        right = x;
    }
    if (bottom > y) {
        bottom = y;
    }
    if (top < y) {
        top = y;
    }
}
CFX_FloatRect CFX_FloatRect::GetBBox(const CFX_FloatPoint* pPoints, int nPoints)
{
    if (nPoints == 0) {
        return CFX_FloatRect();
    }
    FX_FLOAT min_x = pPoints->x, max_x = pPoints->x, min_y = pPoints->y, max_y = pPoints->y;
    for (int i = 1; i < nPoints; i ++) {
        if (min_x > pPoints[i].x) {
            min_x = pPoints[i].x;
        }
        if (max_x < pPoints[i].x) {
            max_x = pPoints[i].x;
        }
        if (min_y > pPoints[i].y) {
            min_y = pPoints[i].y;
        }
        if (max_y < pPoints[i].y) {
            max_y = pPoints[i].y;
        }
    }
    return CFX_FloatRect(min_x, min_y, max_x, max_y);
}
void CFX_Matrix::Set(FX_FLOAT aValue, FX_FLOAT bValue, FX_FLOAT cValue, FX_FLOAT dValue, FX_FLOAT eValue, FX_FLOAT fValue)
{
    this->a = aValue;
    this->b = bValue;
    this->c = cValue;
    this->d = dValue;
    this->e = eValue;
    this->f = fValue;
}
void CFX_Matrix::Set(const FX_FLOAT n[6])
{
    this->a = n[0];
    this->b = n[1];
    this->c = n[2];
    this->d = n[3];
    this->e = n[4];
    this->f = n[5];
}
void CFX_Matrix::SetReverse(const CFX_Matrix &m)
{
    if (this == &m) {
        CFX_Matrix tmp = m;
        SetReverse(tmp);
        return;
    }
    FX_FLOAT i = m.a * m.d - m.b * m.c;
    if (FXSYS_fabs(i) == 0) {
        return;
    }
    FX_FLOAT j = -i;
    a = m.d / i;
    b = m.b / j;
    c = m.c / j;
    d = m.a / i;
    e = (m.c * m.f - m.d * m.e) / i;
    f = (m.a * m.f - m.b * m.e) / j;
}
static void FXCRT_Matrix_Concat(CFX_Matrix &m, const CFX_Matrix &m1, const CFX_Matrix &m2)
{
    FX_FLOAT aa = m1.a * m2.a + m1.b * m2.c;
    FX_FLOAT bb = m1.a * m2.b + m1.b * m2.d;
    FX_FLOAT cc = m1.c * m2.a + m1.d * m2.c;
    FX_FLOAT dd = m1.c * m2.b + m1.d * m2.d;
    FX_FLOAT ee = m1.e * m2.a + m1.f * m2.c + m2.e;
    FX_FLOAT ff = m1.e * m2.b + m1.f * m2.d + m2.f;
    m.a = aa, m.b = bb, m.c = cc, m.d = dd, m.e = ee, m.f = ff;
}
void CFX_Matrix::Concat(FX_FLOAT aValue, FX_FLOAT bValue, FX_FLOAT cValue, FX_FLOAT dValue, FX_FLOAT eValue, FX_FLOAT fValue, FX_BOOL bPrepended)
{
    CFX_Matrix m;
    m.Set(aValue, bValue, cValue, dValue, eValue, fValue);
    Concat(m, bPrepended);
}
void CFX_Matrix::Concat(const CFX_Matrix &m, FX_BOOL bPrepended)
{
    if (this == &m) {
        CFX_Matrix tmp = m;
        Concat(tmp, bPrepended);
        return;
    }
    if (bPrepended) {
        FXCRT_Matrix_Concat(*this, m, *this);
    } else {
        FXCRT_Matrix_Concat(*this, *this, m);
    }
}
void CFX_Matrix::ConcatInverse(const CFX_Matrix& src, FX_BOOL bPrepended)
{
    CFX_Matrix m;
    m.SetReverse(src);
    Concat(m, bPrepended);
}
FX_BOOL CFX_Matrix::IsInvertible() const
{
    return FXSYS_fabs(a * d - b * c) >= 0.0001f;
}
FX_BOOL CFX_Matrix::Is90Rotated() const
{
    return FXSYS_fabs(a * 1000) < FXSYS_fabs(b) && FXSYS_fabs(d * 1000) < FXSYS_fabs(c);
}
FX_BOOL CFX_Matrix::IsScaled() const
{
    return FXSYS_fabs(b * 1000) < FXSYS_fabs(a) && FXSYS_fabs(c * 1000) < FXSYS_fabs(d);
}
void CFX_Matrix::Translate(FX_FLOAT x, FX_FLOAT y, FX_BOOL bPrepended)
{
    if (bPrepended) {
        e += x * a + y * c;
        f += y * d + x * b;
    } else {
        e += x, f += y;
    }
}
void CFX_Matrix::Scale(FX_FLOAT sx, FX_FLOAT sy, FX_BOOL bPrepended)
{
    a *= sx, d *= sy;
    if (bPrepended) {
        b *= sx;
        c *= sy;
    } else {
        b *= sy;
        c *= sx;
        e *= sx;
        f *= sy;
    }
}
void CFX_Matrix::Rotate(FX_FLOAT fRadian, FX_BOOL bPrepended)
{
    FX_FLOAT cosValue = FXSYS_cos(fRadian);
    FX_FLOAT sinValue = FXSYS_sin(fRadian);
    CFX_Matrix m;
    m.Set(cosValue, sinValue, -sinValue, cosValue, 0, 0);
    if (bPrepended) {
        FXCRT_Matrix_Concat(*this, m, *this);
    } else {
        FXCRT_Matrix_Concat(*this, *this, m);
    }
}
void CFX_Matrix::RotateAt(FX_FLOAT fRadian, FX_FLOAT dx, FX_FLOAT dy, FX_BOOL bPrepended)
{
    Translate(dx, dy, bPrepended);
    Rotate(fRadian, bPrepended);
    Translate(-dx, -dy, bPrepended);
}
void CFX_Matrix::Shear(FX_FLOAT fAlphaRadian, FX_FLOAT fBetaRadian, FX_BOOL bPrepended)
{
    CFX_Matrix m;
    m.Set(1, FXSYS_tan(fAlphaRadian), FXSYS_tan(fBetaRadian), 1, 0, 0);
    if (bPrepended) {
        FXCRT_Matrix_Concat(*this, m, *this);
    } else {
        FXCRT_Matrix_Concat(*this, *this, m);
    }
}
void CFX_Matrix::MatchRect(const CFX_FloatRect& dest, const CFX_FloatRect& src)
{
    FX_FLOAT fDiff = src.left - src.right;
    a = FXSYS_fabs(fDiff) < 0.001f ? 1 : (dest.left - dest.right) / fDiff;
    fDiff = src.bottom - src.top;
    d = FXSYS_fabs(fDiff) < 0.001f ? 1 : (dest.bottom - dest.top) / fDiff;
    e = dest.left - src.left * a;
    f = dest.bottom - src.bottom * d;
    b = 0;
    c = 0;
}
FX_FLOAT CFX_Matrix::GetXUnit() const
{
    if (b == 0) {
        return (a > 0 ? a : -a);
    }
    if (a == 0) {
        return (b > 0 ? b : -b);
    }
    return FXSYS_sqrt(a * a + b * b);
}
FX_FLOAT CFX_Matrix::GetYUnit() const
{
    if (c == 0) {
        return (d > 0 ? d : -d);
    }
    if (d == 0) {
        return (c > 0 ? c : -c);
    }
    return FXSYS_sqrt(c * c + d * d);
}
void CFX_Matrix::GetUnitRect(CFX_RectF &rect) const
{
    rect.left = rect.top = 0;
    rect.width = rect.height = 1;
    TransformRect(rect);
}
CFX_FloatRect CFX_Matrix::GetUnitRect() const
{
    CFX_FloatRect rect(0, 0, 1, 1);
    rect.Transform((const CFX_Matrix*)this);
    return rect;
}
FX_FLOAT CFX_Matrix::GetUnitArea() const
{
    FX_FLOAT A = FXSYS_sqrt(a * a + b * b);
    FX_FLOAT B = FXSYS_sqrt(c * c + d * d);
    FX_FLOAT ac = a + c, bd = b + d;
    FX_FLOAT C = FXSYS_sqrt(ac * ac + bd * bd);
    FX_FLOAT P = (A + B + C ) / 2;
    return FXSYS_sqrt(P * (P - A) * (P - B) * (P - C)) * 2;
}
FX_INT32 CFX_Matrix::GetRotation() const
{
    CFX_VectorF vA, vB, vC, vD;
    vA.Set(0.0, 0.0);
    vB.Set(1.0, 0.0);
    vC.Set(1.0, 1.0);
    vD.Set(0.0, 1.0);
    TransformVector(vA);
    TransformVector(vB);
    TransformVector(vC);
    TransformVector(vD);
    CFX_VectorF vAB, vAD;
    vAB.x = vB.x - vA.x;
    vAB.y = vB.y - vA.y;
    vAD.x = vD.x - vA.x;
    vAD.y = vD.y - vA.y;
    if (FXSYS_fabs(vAB.y) < 1.0 && vAB.x > 0.0 && FXSYS_fabs(vAD.x) < 1.0 && vAD.y < 0.0) {
        return 0;
    }
    if (FXSYS_fabs(vAB.x) < 1.0 && vAB.y > 0.0 && FXSYS_fabs(vAD.y) < 1.0 && vAD.x > 0.0) {
        return 1;
    }
    if (FXSYS_fabs(vAB.y) < 1.0 && vAB.x < 0.0 && FXSYS_fabs(vAD.x) < 1.0 && vAD.y > 1.0) {
        return 2;
    }
    if (FXSYS_fabs(vAB.x) < 1.0 && vAB.y < 0.0 && FXSYS_fabs(vAD.y) < 1.0 && vAD.x < 0.0) {
        return 3;
    }
    if (FXSYS_fabs(vAB.y) < 1.0 && vAB.x < 0.0 && FXSYS_fabs(vAD.x) < 1.0 && vAD.y < 0.0) {
        return 4;
    }
    if (FXSYS_fabs(vAB.y) < 1.0 && vAB.x > 0.0 && FXSYS_fabs(vAD.x) < 1.0 && vAD.y > 0.0) {
        return 5;
    }
    return -1;
}
FX_BOOL CFX_Matrix::NeedTransform() const
{
    if (FXSYS_fabs(b) > 0.5f || FXSYS_fabs(a) < 0.5 || FXSYS_fabs(c) > 0.5f || FXSYS_fabs(d) < 0.5) {
        return TRUE;
    } else {
        return FALSE;
    }
}
FX_FLOAT CFX_Matrix::TransformXDistance(FX_FLOAT dx) const
{
    FX_FLOAT fx = a * dx, fy = b * dx;
    return FXSYS_sqrt(fx * fx + fy * fy);
}
FX_INT32 CFX_Matrix::TransformXDistance(FX_INT32 dx) const
{
    FX_FLOAT fx = a * dx, fy = b * dx;
    return FXSYS_round(FXSYS_sqrt(fx * fx + fy * fy));
}
FX_FLOAT CFX_Matrix::TransformYDistance(FX_FLOAT dy) const
{
    FX_FLOAT fx = c * dy, fy = d * dy;
    return FXSYS_sqrt(fx * fx + fy * fy);
}
FX_INT32 CFX_Matrix::TransformYDistance(FX_INT32 dy) const
{
    FX_FLOAT fx = c * dy, fy = d * dy;
    return FXSYS_round(FXSYS_sqrt(fx * fx + fy * fy));
}
FX_FLOAT CFX_Matrix::TransformDistance(FX_FLOAT dx, FX_FLOAT dy) const
{
    FX_FLOAT fx = a * dx + c * dy, fy = b * dx + d * dy;
    return FXSYS_sqrt(fx * fx + fy * fy);
}
FX_INT32 CFX_Matrix::TransformDistance(FX_INT32 dx, FX_INT32 dy) const
{
    FX_FLOAT fx = a * dx + c * dy, fy = b * dx + d * dy;
    return FXSYS_round(FXSYS_sqrt(fx * fx + fy * fy));
}
FX_FLOAT CFX_Matrix::TransformDistance(FX_FLOAT distance) const
{
    return distance * (GetXUnit() + GetYUnit()) / 2;
}
void CFX_Matrix::TransformVector(CFX_VectorF &v) const
{
    FX_FLOAT fx = a * v.x + c * v.y;
    FX_FLOAT fy = b * v.x + d * v.y;
    v.x = fx, v.y = fy;
}
void CFX_Matrix::TransformVector(CFX_Vector &v) const
{
    FX_FLOAT fx = a * v.x + c * v.y;
    FX_FLOAT fy = b * v.x + d * v.y;
    v.x = FXSYS_round(fx);
    v.y = FXSYS_round(fy);
}
void CFX_Matrix::TransformPoints(CFX_Point *points, FX_INT32 iCount) const
{
    FXSYS_assert(iCount > 0);
    FX_FLOAT fx, fy;
    for (FX_INT32 i = 0; i < iCount; i ++) {
        fx = a * points->x + c * points->y + e;
        fy = b * points->x + d * points->y + f;
        points->x = FXSYS_round(fx);
        points->y = FXSYS_round(fy);
        points ++;
    }
}
void CFX_Matrix::TransformPoints(CFX_PointF *points, FX_INT32 iCount) const
{
    FXSYS_assert(iCount > 0);
    FX_FLOAT fx, fy;
    for (FX_INT32 i = 0; i < iCount; i ++) {
        fx = a * points->x + c * points->y + e;
        fy = b * points->x + d * points->y + f;
        points->x = fx, points->y = fy;
        points ++;
    }
}
void CFX_Matrix::TransformPoint(FX_FLOAT &x, FX_FLOAT &y) const
{
    FX_FLOAT fx = a * x + c * y + e;
    FX_FLOAT fy = b * x + d * y + f;
    x = fx, y = fy;
}
void CFX_Matrix::TransformPoint(FX_INT32 &x, FX_INT32 &y) const
{
    FX_FLOAT fx = a * x + c * y + e;
    FX_FLOAT fy = b * x + d * y + f;
    x = FXSYS_round(fx);
    y = FXSYS_round(fy);
}
void CFX_Matrix::TransformRect(CFX_RectF &rect) const
{
    FX_FLOAT right = rect.right(), bottom = rect.bottom();
    TransformRect(rect.left, right, bottom, rect.top);
    rect.width = right - rect.left;
    rect.height = bottom - rect.top;
}
void CFX_Matrix::TransformRect(CFX_Rect &rect) const
{
    FX_FLOAT left = (FX_FLOAT)rect.left;
    FX_FLOAT top = (FX_FLOAT)rect.bottom();
    FX_FLOAT right = (FX_FLOAT)rect.right();
    FX_FLOAT bottom = (FX_FLOAT)rect.top;
    TransformRect(left, right, top, bottom);
    rect.left = FXSYS_round(left);
    rect.top = FXSYS_round(bottom);
    rect.width = FXSYS_round(right - left);
    rect.height = FXSYS_round(top - bottom);
}
void CFX_Matrix::TransformRect(FX_FLOAT& left, FX_FLOAT& right, FX_FLOAT& top, FX_FLOAT& bottom) const
{
    FX_FLOAT x[4], y[4];
    x[0] = left;
    y[0] = top;
    x[1] = left;
    y[1] = bottom;
    x[2] = right;
    y[2] = top;
    x[3] = right;
    y[3] = bottom;
    int i;
    for (i = 0; i < 4; i ++) {
        Transform(x[i], y[i], x[i], y[i]);
    }
    right = left = x[0];
    top = bottom = y[0];
    for (i = 1; i < 4; i ++) {
        if (right < x[i]) {
            right = x[i];
        }
        if (left > x[i]) {
            left = x[i];
        }
        if (top < y[i]) {
            top = y[i];
        }
        if (bottom > y[i]) {
            bottom = y[i];
        }
    }
}
