# ORANGE Domain Translation
#    by Alex Jakulin (jakulin@acm.org)
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
# CVS Status: $Id$
#
# Version 2.0 (20/11/2003)
#   - SVM no longer uses -1 for minimum value in categorical attributes
#
# Version 1.9 (17/11/2003)
#   - Dummy picks the most frequent value to be the default
#   - added standardization
#   - fixed a bug in binarization
#
# Version 1.8 (10/7/2003)
#   - support for domain disparity
#
# Version 1.7 (14/8/2002)
#   - status functions
#   - Support for setting the preferred ordinal attribute transformation.
#
# Version 1.6 (31/10/2001)
#
#
# TO DO:
#   - Currently dummy variables are created for all discrete attributes, but
#     often it would be more desirable, especially for ordinal attributes,
#     to have other methods of creating continuous attributes. Perhaps use
#     MDS for this.
#

import orange, warnings, math

#
# MAPPERS translate orange domains into primitive python arrays used by the
#                                                           LR/SVM functions
#
# note that these mappings are extremely SLOW and INEFFICIENT
#
#
# Scalizer maps continuous input into an interval ([-1,1] for SVM, [0,1] for LR)
# Ordinalizer maps is a scalizer for ordinal attributes, assuming even spread.
#
# Binarizer and Dummy create several new attributes:
# Binarizer maps like this <1,2,3> -> <[1,0,0],[0,1,0],[0,0,1]>
# Dummy maps like this <1,2,3> -> <[1,0],[0,1],[0,0]>
#
# The default operators are scalizer (cont) and dummy (discr);
# For class, ordinalizer is used instead of dummy
#
# if you are unhappy about this, subclass DomainTranslation and fudge with analyse()

def _getattr(ex,attr):
    # a 'smart' function tries to access an attribute first by reference, then by name
    spec = 0
    try:
        v = ex[attr]
    except:
        # perhaps the domain changed
        try:
            v = ex[attr.name]
        except:
            warnings.warn("Missing attribute %s"%attr.name)
            v = attr.values[0]
            spec = 1            
    if spec == 0:
        try:
            spec = value.isSpecial()
        except:
            pass
    return (v,spec)

class Scalizer:
    def __init__(self,idx,attr,isclass=0):
        self.min = 1e200
        self.max = -1e200
        self.idx = idx
        self.nidx = idx+1
        self.attr= attr
        self.isclass = isclass

    def learn(self,value):
        try:
            spec = value.isSpecial()
        except:
            spec = 0
        if not spec:
            value = float(value)
            if self.min > value:
                self.min = value
            if self.max < value:
                self.max = value

    def status(self):
        print "scalizer: "
        print "\tattr:",self.attr
        print "\tidx: ",self.idx
        print "\tmin: ",self.min
        print "\tmax: ",self.max
            
    def prepareSVM(self):
        if self.isclass == 1:
            self.missing = 3.14159   # a special value!
        else:
            self.missing = (0,1)
        if self.min == self.max:
            if self.max > 0.0 or self.max < 0.0 :
                self.mult = 1.0/self.max
                self.disp = 0.0
            else:
                self.mult = 1.0
                self.disp = self.max
        else:
            self.mult = 2.0/(self.max-self.min)
            self.disp = (self.min+self.max)/2.0

    def activate(self):
        pass

    def prepareLR(self):
        self.missing = (self.min+self.max)/2
        self.mult = 1
        self.disp = 0
        
    def apply(self,ex,list):
        (value,spec) = _getattr(ex,self.attr)
        if spec:
            list[self.idx] = self.missing
        else:
            list[self.idx] = (float(value)-self.disp)*self.mult
        return

    def description(self):
        if self.disp==0.0 and self.mult==1:
            return ['%s'%(self.attr.name)]
        else:
            return ['%s(x)=(x-%f)*%f'%(self.attr.name,self.disp,self.mult)]

    def description(self):
        return (0,'%s'%(self.attr.name))

    def inverse(self,list):
        return self.attr((list[self.idx]/self.mult)+self.disp)
        

class Standardizer:
    def __init__(self,idx,attr,isclass=0):
        self.avg = 0.0
        self.stddev = 0.0
        self.idx = idx
        self.nidx = idx+1
        self.attr= attr
        self.values = []
        self.isclass = isclass

    def learn(self,value):
        try:
            spec = value.isSpecial()
        except:
            spec = 0
        if not spec:
            value = float(value)
            self.values.append(value)
            self.avg += value

    def activate(self):
        pass

    def status(self):
        print "standardizer: "
        print "\tattr:",self.attr
        print "\tidx: ",self.idx
        print "\taverage: ",self.avg
        print "\tstddev: ",self.stddev
            
    def prep(self):
        self.avg /= len(self.values)
        for x in self.values:
            t = x-self.avg
            self.stddev += t*t
        self.stddev /= len(self.values)-1
        self.stddev = math.sqrt(self.stddev)
        self.mult = 1.0/self.stddev
        self.disp = self.avg

    def prepareSVM(self):
        self.prep()
        if self.isclass == 1:
            self.missing = 3.14159   # a special value!
        else:
            self.missing = (0,1)

    def prepareLR(self):
        self.prep()
        self.mult = 1.0
        self.missing = 0.0
        
    def apply(self,ex,list):
        (value,spec) = _getattr(ex,self.attr)
        if spec:
            list[self.idx] = self.missing
        else:
            list[self.idx] = (float(value)-self.disp)*self.mult
        return

    def descript(self):
        if self.disp==0.0 and self.mult==1.0:
            return ['%s'%(self.attr.name)]
        else:
            return ['(%s-%f)*%f'%(self.attr.name,self.disp,self.mult)]

    def description(self):
        return (0,'%s'%(self.attr.name))

    def inverse(self,list):
        return self.attr((list[self.idx]/self.mult)+self.disp)


class Ordinalizer:
    def __init__(self,idx,attr,isclass=0):
        self.idx = idx
        self.nidx = idx+1
        self.min = 0.0
        self.max = len(attr.values)-1.0
        self.attr = attr
        self.isclass = isclass
        return

    def status(self):
        print "ordinalizer: "
        print "\tattr:",self.attr
        print "\tidx: ",self.idx
        print "\tmin: ",self.min
        print "\tmax: ",self.max
        print "\tisclass:",self.isclass
            
    def learn(self,value):
        return
            
    def activate(self):
        pass

    def prepareSVM(self):
        if self.isclass==1:
            # keep all classes integer, because libsvm does rounding!!!
            self.mult = 1.0
            self.disp = 0.0
        else:
            self.missing = (0,1)
            if self.min == self.max:
                if self.max > 0.0 or self.max < 0.0 :
                    self.mult = 1.0/self.max
                    self.disp = 0.0
                else:
                    self.mult = 1.0
                    self.disp = self.max
            else:
                self.mult = 2.0/(self.max-self.min)
                self.disp = (self.min+self.max)/2.0
        return

    def prepareLR(self):
        self.missing = 0.5
        if self.min == self.max:
            if self.max > 0.0 or self.max < 0.0 :
                self.mult = 1.0/self.max
                self.disp = 0.0
            else:
                self.mult = 1.0
                self.disp = self.max
        else:
            self.mult = 1.0/(self.max-self.min)
            self.disp = self.min
        return
        
    def apply(self,ex,list):
        (value,spec) = _getattr(ex,self.attr)
        if spec:
            list[self.idx] = self.missing
        else:
            list[self.idx] = (int(value)-self.disp)*self.mult
        return

    def descript(self):
        if self.disp==0 and self.mult==1:
            return ['%s'%(self.attr.name)]
        else:
            return ['%s(x)=(x-%f)*%f'%(self.attr.name,self.disp,self.mult)]

    def description(self):
        return (1,'%s'%(self.attr.name),['%s'%(v) for v in self.attr.values])

    def inverse(self,list):
        return self.attr(int((list[self.idx]/self.mult)+self.disp+0.5))

class Binarizer:
    def __init__(self,idx,attr):
        self.idx = idx
        self.nidx = idx+len(attr.values)
        self.attr = attr
        return

    def status(self):
        print "binarizer: "
        print "\tattr:",self.attr
        print "\tidx: ",self.idx
        print "\tidxn:",self.nidx-self.idx
            
    def learn(self,value):
        return
            
    def prepareSVM(self):
        self.min = 0.0
        self.max = 1.0
        self.missing = (0,1)
        return

    def prepareLR(self):
        self.min = 0.0
        self.max = 1.0
        self.missing = 1.0/(self.nidx-self.idx)
        return

    def activate(self):
        pass
    
    def apply(self,ex,list):
        (value,spec) = _getattr(ex,self.attr)
        if spec:
            for i in range(self.idx,self.nidx):
                list[i] = self.missing
        else:
            for i in range(self.idx,self.nidx):
                list[i] = self.min
            list[self.idx+int(value)] = self.max

    def descript(self):
        return ['%s=%s'%(self.attr.name,v) for v in self.attr.values]

    def description(self):
        return (1,'%s'%(self.attr.name),['%s'%(v) for v in self.attr.values])

    def inverse(self,list):
        best = -1
        bestv = 1e200
        for i in range(self.idx,self.nidx):
            val = abs(list[self.idx]-self.max)
            if val < bestv:
                bestv = val
                best = i
        assert(best>=0)
        return self.attr(best-self.idx)

class Dummy:
    def __init__(self,idx,attr):
        self.idx = idx
        self.nidx = idx+len(attr.values)-1
        self.counts = [0]*len(attr.values)
        self.attr = attr
        return

    def learn(self,value):
        if not value.isSpecial():
            self.counts[int(value)] += 1
        return

    def activate(self):
        # identify the most frequent one
        i = 0
        maxx = max(self.counts)
        self.maxi = -1
        self.lut = [-1]*len(self.counts)
        for x in range(len(self.counts)):
            if self.counts[x]==maxx and self.maxi < 0:
                self.maxi = x
            else:
                self.lut[x] = i
                i += 1
        
                
    def status(self):
        print "dummy: "
        print "\tattr:",self.attr
        print "\tidx: ",self.idx
        print "\tidxn:",self.nidx-self.idx
            
    def prepareSVM(self):
        self.min = 0.0
        self.max = 1.0
        self.missing = (0,1)
        return

    def prepareLR(self):
        self.min = 0.0
        self.max = 1.0
        self.missing = 0.0
        return
    
    def apply(self,ex,list):
        (value,spec) = _getattr(ex,self.attr)
        if spec:
            # missing value handling
            for i in range(self.idx,self.nidx):
                list[self.idx] = self.missing
        else:
            for i in range(self.idx,self.nidx):
                list[self.idx] = self.min
            i = self.lut[int(value)]
            if i != -1:
                list[self.idx+i] = self.max

    def descript(self):
        d = []
        for x in range(len(self.attr.values)):
            if self.lut[x] != -1:
                d.append('%s=%s'%(self.attr.name,self.attr.values[x]))
        return d 

    def description(self):
        d = []
        for x in range(len(self.attr.values)):
            if self.lut[x] != -1:
                d.append('%s'%(self.attr.values[x]))
            else:
                d.append('')
        return (1,'%s'%(self.attr.name),d)

    def inverse(self,list):
        best = self.nidx
        bestv = 1e200
        for i in range(self.idx,self.nidx):
            val = abs(list[self.idx]-self.max)
            if val < bestv:
                bestv = val
                best = i
        return self.attr(best-self.idx)

class DomainTranslation:
    def __init__(self, mode = 0):
        # 0: always dummy
        # 1: always binarize
        # 2: binarize if more values than 2, else dummy
        self.mode = mode

    def analyse(self,examples,weight=0):
        # attributes
        self.trans = []
        self.weight = weight
        idx = 0
        for i in examples.domain.attributes:
            if i.varType == 2:
                # continuous
                t = Standardizer(idx,i)
            else:
                if i.varType == 1:
                    
                    if self.mode == 0:
                        t = Dummy(idx,i)
                    elif self.mode == 1:
                        t = Binarizer(idx,i)
                    elif self.mode == 2:
                        if len(i.values) > 2:
                            t = Binarizer(idx,i)
                        else:
                            t = Dummy(idx,i)
            self.trans.append(t)
            idx = t.nidx

        # class: in its own array
        i = examples.domain.classVar
        if i.varType == 2:
            # continuous
            self.cv = Scalizer(0,i,isclass=1)
        else:
            if i.varType == 1:
                # discrete
                if len(i.values) > 2:
                    warnings.warn("Simulating classification with regression. It's better to use orngMultiClass!")
                self.cv = Ordinalizer(0,i,isclass=1)

        # learning the properties of transformers
        for j in examples:
            for i in range(len(self.trans)):
                self.trans[i].learn(j[i])
            self.cv.learn(j.getclass())

        # do the final preparations
        for i in range(len(self.trans)):
            self.trans[i].activate()

    def prepareLR(self):
        # preparation
        self.cv.prepareLR()
        self.missing = 1
        self.weights = 1
        for i in self.trans:
            i.prepareLR()

    def prepareSVM(self):
        # preparation
        self.cv.prepareSVM()
        self.missing = 0
        self.weights = 0 # SVM is not compatible with example weighting
        for i in self.trans:
            i.prepareSVM()

    def transformClass(self, classvector):
        # used for getting the label list
        r = []
        for i in classvector:
            newc = [0.0]
            x = orange.Example(orange.Domain([self.cv.attr]),[i])
            self.cv.apply(x,newc)
            r.append(newc[0])
        return r
    
    def transform(self,examples):
        # transformation of examples
        newt = []
        for j in examples:
            newv = [0.0]*self.trans[-1].nidx
            newc = [0.0]
            for i in range(len(self.trans)):
                self.trans[i].apply(j,newv)
            self.cv.apply(j,newc)

            # process weight
            if self.weight != 0 and self.weights != 0:
                newc = [(newc[0],j.getmeta(self.weight))]

            newt.append(newv + newc)
        return newt

    def extransform(self,example):
            newv = [0.0]*self.trans[-1].nidx
            for i in range(len(self.trans)):
                self.trans[i].apply(example,newv)
            return newv

    def getClass(self,label):
        # inverse transformation
        return self.cv.inverse([label])
    
    def status(self):
        print "Attributes:"
        for i in self.trans:
            print i.status()
        print "Class:"
        print self.cv.status()
        print "dummy: "            

    def description(self):
        ds = []
        for i in range(len(self.trans)):
            ds += self.trans[i].description()
        return (ds,self.cv.description())