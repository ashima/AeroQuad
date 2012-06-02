#!/usr/bin/env python
"""Orthogonalizes and normalizes a rotation-like matrix using sympy routines"""

from sympy.matrices import *
from sympy import sqrt, Matrix, latex, Symbol
from numpy import array

def rotation_3d(costheta,sintheta,vec):
    """Generate a 3d rotation matrix of angle theta around vector vec"""
    ux,uy,uz = vec
    omcostheta = 1.0 - costheta
    return Matrix([ [costheta + ux*ux*omcostheta, ux*uy*omcostheta - uz*sintheta, ux*uz*omcostheta + uy*sintheta],
                    [uy*ux*omcostheta + uz*sintheta, costheta+uy*uy*omcostheta, uy*uz*omcostheta-ux*sintheta],
                    [uz*ux*omcostheta - uy*sintheta, uz*uy*omcostheta + ux*sintheta, costheta + uz*uz*omcostheta]])


def orthogonalize(x,y,z,verbose=False):
    """Takes a rotation-like matrix and return the constrained orthogonal+normalized rotation matrix"""
    #1. define some vectors
    ivec = Matrix([1.,0,0])
    jvec = Matrix([0.,1,0])
    kvec = Matrix([0.,0,1])
    
    #2. take the cross product of x, y to get z
    znorm = (x.cross(y)).normalized()
    print "z_\mathrm{norm} =",latex(znorm),"\\"
    
    #3. calculate angles
    cos_theta = znorm.dot(kvec)
    if znorm.dot(kvec) != 1.0:
        czk = znorm.cross(kvec)
        sin_theta = czk.norm()
        czk = czk / sin_theta
    else:
        print "Error"
        return

    print "cos\\theta ={0}\\".format(latex(cos_theta))
    sin_theta = sqrt(1-cos_theta*cos_theta)
    print "sin\\theta ={0}\\".format(latex(sin_theta))

    #4. make the rotation matrix
    R_theta = rotation_3d(cos_theta, sin_theta, czk)
    print "R\\theta ={0}\\".format(latex(R_theta))

    #5. generate v, and rotate to v'
    v = x + y
    vprime = (R_theta*v.T).normalized()
    print "vprime ={0}\\".format(latex(vprime))

    #6. generate psi angles for rotation around kvec
    cos_psi = vprime.dot(ivec)
    sin_psi = sqrt(1-cos_psi**2)
    print "sin\psi ={0}\\".format(latex(sin_psi))
    print "cos\psi ={0}\\".format(latex(cos_psi))
    R_psi = rotation_3d(cos_psi, sin_psi, czk)

    #generate the 45 degree rotation.
    print "R\psi = {0}".format(latex(R_psi))
    cospi4 = 1./sqrt(2)
    R_pi4 = rotation_3d(cospi4, cospi4, kvec) #cospi4 = sinpi4
    print "R\pi/4 = {0}\\".format(latex(R_i4))
    M = R_theta.T * R_psi * R_pi4
    print "M = {0}\\".format(latex(M))
    
if __name__ == "__main__":
    a=Matrix([[-1.77683227, -0.47622354,  0.30327778],
           [ 1.62332859,  3.58106104, -1.63407377],
           [ 2.5113281 ,  4.18181844,  2.10494566]])
# The symbolic version does work, but takes a very long time.
#    a=Matrix([[Symbol('a'), Symbol('b'),  Symbol("c")],
#              [Symbol('d'), Symbol('e'),  Symbol("f")],
#              [Symbol('g'), Symbol('h'),  Symbol("i")]])

    x=a[0,:]
    y=a[1,:]
    z=a[2,:]
    
    orthogonalize(x,y,z)
