#!/usr/bin/env python
"""Trivial program that calculates the product of 3 3d rotation matrices using symbolic python.
Requires sumpy and numpy.
"""

from sympy.matrices import *
from sympy import sqrt, Matrix, latex, Symbol, sin,cos
from numpy import array

def rotation_3d(costheta,sintheta,vec):
    """Generate a 3d rotation matrix with angle theta around vector vec"""
    ux,uy,uz = vec
    omcostheta = 1.0 - costheta
    return Matrix([ [costheta + ux*ux*omcostheta, ux*uy*omcostheta - uz*sintheta, ux*uz*omcostheta + uy*sintheta],
                    [uy*ux*omcostheta + uz*sintheta, costheta+uy*uy*omcostheta, uy*uz*omcostheta-ux*sintheta],
                    [uz*ux*omcostheta - uy*sintheta, uz*uy*omcostheta + ux*sintheta, costheta + uz*uz*omcostheta]])

if __name__ == "__main__":
    p=Symbol("p")
    q=Symbol("q")
    r=Symbol("r")
    u=Symbol("u")
    v=Symbol("v")
    sp,cp = sin(p), cos(p)
    sq,cq = sin(q), cos(q)
    sr,cr = sin(r), cos(r)
    psi =rotation_3d(cq, sq, Matrix([0,0,1]))
    phi = rotation_3d(cp, sp, Matrix([0,0,1]))
    theta = rotation_3d(cr, sr, Matrix([u,-v,0]))
    print phi*psi
    print theta
    print theta.T*phi*psi
