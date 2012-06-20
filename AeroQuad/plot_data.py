#!/usr/bin/env python
import numpy as np
import matplotlib as mpl
mpl.use("Agg")
import pylab as pl

data = np.loadtxt("data").T
data[1]=data[1]/100.
data[2]=data[2]/50.
data[3]=data[3]/100.
data[4]=data[4]/100.

width = 0.35
pl.figure(figsize=(8,8))
p1=pl.bar(data[0], data[1], width, color='r')
p2=pl.bar(data[0],data[2], width,bottom=data[1], color='g')
p3=pl.bar(data[0],data[3], width,bottom=data[1]+data[2], color='b')
print data[4] - (data[1]+data[2]+data[3]), data[:,1]
p4=pl.bar(data[0], data[4]-(data[1]+data[2]+data[3]), width, bottom=data[1]+data[2]+data[3], color='grey')
pl.xlabel("10Hz step")
pl.xticks(np.arange(10))
pl.ylabel("Time (milliseconds)")
pl.legend((p1[0],p2[0],p3[0],p4[0]),('100Hz','50Hz','10Hz','Ancillary'), loc='upper center')
pl.savefig("a.pdf")
