#!/usr/bin/env python
""""Program to take a 3d matrix given as a comma separated list of 9 elements and convert that
matrix into a true rotation matrix by finding the normal to the xy plane (the Z axis) and generation two orthogonal axes (X and Y) each 45 degrees from the mean of x and y on the xy plane)."""

from numpy import dot, cross, sqrt, array, cos, sin, matrix, deg2rad, random, pi
from numpy.linalg import det
import numpy as np

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

def isrotation(mat):
    test = True
    test = test and (np.abs(det(mat)-1) < 1e-5)
    test = test and (np.abs(dot(mat[0],mat[0].T)-1) < 1e-5)
    test = test and (np.abs(dot(mat[1],mat[1].T)-1) < 1e-5)
    test = test and (np.abs(dot(mat[2],mat[2].T)-1) < 1e-5)
    if type(test) is matrix:
        return array(test)[0,0]
    else:
        return test


def orthogonalize_alt(mat,verbose=False):
    """Uses Ian's patented method"""
    x,y,z=array(mat)[0], array(mat)[1], array(mat)[2]

    #calculate znorm as normalized(x .cross. y)
    zprime = normalize(cross(x,y))
    
    #calculate the mean of x+y
    v = normalize(x+y)
    
    #calculate v' = 1./sqrt(2)v
    isqrt2 = 0.707106781
    vprime = isqrt2 * v

    #calculate 
    c = dot(zprime,vprime-v)
    a = cross(zprime, vprime)
    b = c*z + vprime
    x = b - a
    y = b + a
    
    M = matrix([x,y,zprime]).T
    return dict(zprime=zprime, 
                v=v, vprime=vprime, c=c,
                a=a, b=b,
                x=x,
                y=y, 
                M=M)
         
def DCM(mat, verbose=False):
    x,y,z=array(mat)[0], array(mat)[1], array(mat)[2]
    #calc error
    error = -dot(x,y)*0.5
    
    ny = y+error*x
    nx = x+error*y
    
    #cross product to get z
    nz = cross(x,y)
    
    #approximately normalize
    renorm = 0.5*(3-dot(nx,nx))
    nx = nx*renorm
    renorm = 0.5*(3-dot(ny,ny))
    ny = ny*renorm
    renorm = 0.5*(3-dot(nz,nz))
    nz = nz*renorm

    M = matrix([nx,ny,nz]).T

    return dict(nx=nx, ny=ny, nz=nz, M=M, error=error)
    

def orthogonalize(mat, verbose=False):
    """Takes a 3d matrix defined by row vectors x,y,z (the three axis of the orientation matrix) and generates a matrix constrained to be a true rotation matrix M, with M^T.M = I, |M| = 1, M_i.M_j = 0 i!=j, M_ixM_j = M_k for i,j,k=1,2,3 or rotational varients of it"""
    x,y,z=array(mat)[0], array(mat)[1], array(mat)[2]
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
#        print "cross  =", cross(xnorm, ynorm), cross(xnorm, znorm), cross(ynorm, znorm)
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
                znorm=znorm,
                cos_theta=cos_theta, sin_theta=sin_theta,
                czk=czk,
                f=f, h=h, R_theta=R_theta, v=v, vprime=vprime,
                cos_psi=cos_psi, sin_psi=sin_psi, R_psi=R_psi, R_pi4=R_pi4, M=M)

def genRot(noise=0.0, noisefree=False):
    theta = (np.random.rand()*2-1.)*np.pi
    vec=normalize(np.random.rand(3)*2-1.0)
    M=rotation_3d(cos(theta), sin(theta), vec)
    real=M
    M=M+np.random.rand(3,3)*noise
    if noisefree:
        return M, real
    else:
        return M

def testEmpty(noise=0.0):
    M=genRot(noise=noise)
    #r = orthogonalize(M)
    #q = orthogonalize_alt(M)
    return None

def testOrthoMatrix(noise=0.0):
    M=genRot(noise=noise)
    r = orthogonalize(M)
    #q = orthogonalize_alt(M)
    return r
    
def testOrthoIan(noise=0.0):
    M=genRot(noise=noise)
    M=M+np.random.rand(3,3)*0.1
    #r = orthogonalize(M)
    q = orthogonalize_alt(M)
    return q

def testDCM(noise=0.0):
    M=genRot(noise=noise)
    M=M+np.random.rand(3,3)*0.1
    #r = orthogonalize(M)
    s = DCM(M)
    return s

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
