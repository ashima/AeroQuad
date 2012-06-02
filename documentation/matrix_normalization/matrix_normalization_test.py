#!/usr/bin/env python
""""Program to take a 3d matrix given as a comma separated list of 9 elements and convert that
matrix into a true rotation matrix by finding the normal to the xy plane (the Z axis) and generation two orthogonal axes (X and Y) each 45 degrees from the mean of x and y on the xy plane)."""

from numpy import dot, cross, sqrt, array, cos, sin, matrix, deg2rad, random, pi
from numpy.linalg import det

def magnitude(x):
    """returns the 2-norm of the vector complex/real x"""
    return sqrt(dot(x,x.conj()))
    
def normalize(x):
    """returns x/magnitude(x). The vector x_hat"""
    return x/magnitude(x)
    
def r3d(angle, axis):
    """returns a 3d rotation matrix of angle 'angle' in radians around vector 'axis'"""
    return rotation_3d(cos(angle), 1.-cos(angle), sin(angle), axis)
    
def rotation_3d(costheta,sintheta,vec):
    """generates the 3d rotation matrix with angle theta around the vector 'vec'"""
    ux,uy,uz = vec
    omcostheta = 1.0 - costheta
    return matrix([ [costheta + ux*ux*omcostheta, ux*uy*omcostheta - uz*sintheta, ux*uz*omcostheta + uy*sintheta],
                    [uy*ux*omcostheta + uz*sintheta, costheta+uy*uy*omcostheta, uy*uz*omcostheta-ux*sintheta],
                    [uz*ux*omcostheta - uy*sintheta, uz*uy*omcostheta + ux*sintheta, costheta + uz*uz*omcostheta]])


def orthogonalize(x,y,z, verbose=False):
    """Takes a 3d matrix defined by row vectors x,y,z (the three axis of the orientation matrix) and generates a matrix constrained to be a true rotation matrix M, with M^T.M = I, |M| = 1, M_i.M_j = 0 i!=j, M_ixM_j = M_k for i,j,k=1,2,3 or rotational varients of it"""

    #1. generate the i,j,k and x,y axes
    ivec = array([1,0,0.])
    jvec = array([0,1,0.])
    kvec = array([0,0,1.])

    #2. calculate z as cross(x,y)
    znorm = normalize(cross(x, y))

    #3. calculate cosine theta, sine theta
    cos_theta = dot(znorm,kvec)
    if dot(znorm, kvec) !=1.0:
        czk = cross(znorm, kvec)
        sin_theta = magnitude(czk)
        czk = czk / sin_theta
    else: 
        #what happens here?
        #znorm is kvec so we don't rotate the matrix onto kvec, 
        print "error"
        return
    sin_theta = sqrt(1-cos_theta*cos_theta)

    #4. create the rotation matrix
    f = cos_theta
    h = sin_theta
    R_theta = rotation_3d(f,h,czk)

    #5. construct v
    v = x+y

    #6. rotate v to v' MATRIX multiply
    vprime = normalize(array(R_theta * matrix(v).T).squeeze())
    
    #7. psi angles
    cos_psi = dot(vprime, ivec)
    sin_psi = sqrt(1-cos_psi**2) #dot(vprime, jvec)
    
    R_psi = rotation_3d(cos_psi, sin_psi, kvec)
    
    R_pi4 = rotation_3d(1./sqrt(2), -1/sqrt(2), kvec)
    
    M = R_theta.T * R_psi * R_pi4

    if verbose:
        print "x  =", x, magnitude(x)
        print "y  =", y, magnitude(y)
        print "znorm  =", znorm, magnitude(znorm)
        print "xdot   =", dot(x, y), dot(x, znorm), dot(y, znorm)
        print "cross  =", cross(xnorm, ynorm), cross(xnorm, znorm), cross(ynorm, znorm)
        print "theta  =", cos_theta, sin_theta, sqrt(1-cos_theta*cos_theta)
        print "psi    =", cos_psi, sin_psi, sqrt(1-cos_psi*cos_psi)
        print "v      =", v, magnitude(v)
        print "csk    =", czk, magnitude(czk)
        print "vprime =", vprime, magnitude(vprime)
        print "R_theta=\n", R_theta, det(R_theta)
        print "R_psi  =\n", R_psi, det(R_psi)
        print "R_pi4  =\n", R_pi4, det(R_pi4)
        print "M      =\n", M, det(M)
        
    return dict(kvec=kvec, 
                xnorm=xnorm, ynorm=ynorm, znorm=znorm,
                cos_theta=cos_theta, sin_theta=sin_theta,
                czk=czk,
                f=f, g=g, h=h, R_theta=R_theta, v=v, vprime=vprime,
                cos_psi=cos_psi, sin_psi=sin_psi, R_psi=R_psi, R_pi4=R_pi4, M=M)

if __name__=="__main__":
    import sys
    if len(sys.argv) > 1:
        vec = map(lambda x: float(x), sys.argv[1].split(","))
        x = array(vec[0:3])
        y = array(vec[3:6])
        z = array(vec[6:9])
        data = orthogonalize(x,y,z, verbose=True)
    else:
        print "Usage: {0} x1,x2,x3,y1,y2,y3,z1,z2,z3".format(sys.argv[0])
        sys.exit(0)
