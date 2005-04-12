/*
    This file is part of Orange.

    Orange is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Orange is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Orange; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Authors: Janez Demsar, Blaz Zupan, 1996--2002
    Contact: janez.demsar@fri.uni-lj.si
*/

#include "examplegen.hpp"
#include "domain.hpp"
#include "basstat.hpp"

#include "pnn.ppp"

TPNN::TPNN(PDomain domain, const float &e2)
: TClassifierFD(domain, true),
  dimensions(0),
  offsets(),
  normalizers(),
  bases(NULL),
  nExamples(0),
  projections(NULL),
  exponent2(e2)
{}


TPNN::TPNN(PDomain domain, PExampleGenerator egen, double *bases, const float &e2)
{ raiseError("not implemented yet"); }


TPNN::TPNN(PDomain domain, double *examples, const int &nEx, double *ba, const int &dim, PFloatList off, PFloatList norm, const float &e2)
: TClassifierFD(domain),
  dimensions(dim),
  offsets(off),
  normalizers(norm),
  bases((double *)memcpy(new double[domain->attributes->size()*dim], ba, domain->attributes->size()*dim*sizeof(double))),
  nExamples(nEx),
  projections(new double[dim*nEx]),
  exponent2(e2)
{
  const int nAttrs = domain->attributes->size();
  TFloatList::const_iterator offi, offb = offsets->begin(), offe = offsets->end();
  TFloatList::const_iterator nori, norb = normalizers->begin(), nore = normalizers->end();

  double *pi, *pe;
  for(pi = projections, pe = projections + (dim+1)*nEx; pi != pe; *(pi++) = 0.0);

  for(double *example = examples, *examplee = examples + nEx*dimensions, *projection = projections; example != examplee; projection = pe) {
    offi = offb;
    nori = norb;
    pe = projection + dimensions;
    double *base = bases;
    for(double *ee = example + nAttrs; example != ee; example) {
      double aval = (*(example++) - *(offi++)) / *(nori++);
      for(pi = projection; pi != pe; *(pi++) += aval * *(base++));
    }
    *pe++ = *example++; // copy the class
  }
}


TPNN::TPNN(PDomain domain, double *examples, const int &nEx, double *ba, const int &dim, PFloatList off, PFloatList norm, const float &e2, const vector<int> &attrIndices, int &nOrigRow)
: TClassifierFD(domain),
  dimensions(dim),
  offsets(off),
  normalizers(norm),
  bases((double *)memcpy(new double[domain->attributes->size()*dim], ba, domain->attributes->size()*dim*sizeof(double))),
  nExamples(nEx),
  projections(new double[dim*nEx]),
  exponent2(e2)
{
  const int nAttrs = domain->attributes->size();
  TFloatList::const_iterator offi, offb = offsets->begin(), offe = offsets->end();
  TFloatList::const_iterator nori, norb = normalizers->begin(), nore = normalizers->end();

  double *pi, *pe;
  for(pi = projections, pe = projections + (dim+1)*nEx; pi != pe; *(pi++) = 0.0);

  for(double *example = examples, *examplee = examples + nEx*dimensions, *projection = projections; example != examplee; projection = pe, example += nOrigRow) {
    offi = offb;
    nori = norb;
    pe = projection + dimensions;
    double *base = bases;
    const_ITERATE(vector<int>, ai, attrIndices) {
      double aval = (example[*ai] - *(offi++)) / *(nori++);
      for(pi = projection; pi != pe; *(pi++) += aval * *(base++));
    }
    *pe++ = example[nOrigRow-1]; // copy the class
  }
}


TPNN::TPNN(const TPNN &old)
: TClassifierFD(old),
  dimensions(0),
  bases(NULL),
  nExamples(0),
  projections(NULL)
{ *this = old; }


TPNN &TPNN::operator =(const TPNN &old)
{
  if (bases)
    delete bases;

  if (old.bases) {
    const int nAttrs = domain->attributes->size();
    bases = (double *)memcpy(new double[nAttrs*dimensions], old.bases, nAttrs*dimensions*sizeof(double));
  }
  else
    bases = NULL;


  if (projections)
    delete projections;

  if (old.projections)
    projections = (double *)memcpy(new double[nExamples*(dimensions+1)], old.projections, nExamples*(dimensions+1)*sizeof(double));
  else
    projections = NULL;


  if (old.offsets)
    offsets = new TFloatList(old.offsets.getReference());
  else
    offsets = PFloatList();

  if (old.normalizers)
    normalizers = new TFloatList(old.normalizers.getReference());
  else
    normalizers = PFloatList();


  nExamples = old.nExamples;
  exponent2 = old.exponent2;

  return *this;
}


TPNN::~TPNN()
{
  if (bases)
    delete bases;

  if (projections)
    delete projections;

  if (radii)
    delete radii;
}


void TPNN::project(const TExample &example, double *projection)
{
  TFloatList::const_iterator offi = offsets->begin(), nori = normalizers->begin();

  double *pi, *pe = projection + dimensions;
  for(pi = projection; pi != pe; *(pi++) = 0.0);

  double *base = bases;

  for(TExample::const_iterator ei = example.begin(), ee = example.end(); ei != ee; ) {
    if ((*ei).isSpecial())
      raiseError("cannot handle missing values");

    double aval = ((*(ei++)).floatV - *(offi++)) / *(nori++);
    for(pi = projection; pi != pe; *(pi++) += aval * *(base++));
  }
}

PDistribution TPNN::classDistribution(const TExample &example)
{
  double *projection = mlnew double[dimensions];
  double *pe = projection + dimensions;

  const int nClasses = domain->classVar->noOfValues();
  float *cprob = mlnew float[nClasses];
  for(float *ci = cprob, *ce = cprob + nClasses; ci != ce; *(ci++) = 0.0)
  
  try {
    if (example.domain == domain)
      project(example, projection);
    else {
      TExample nex(domain, example);
      project(example, projection);
    }

    for(double *proj = projections, *proje = projections+ nExamples*(dimensions+1); proj != proje; ) {
      double dist = 0.0;
      double *pi = projection;
      while(pi!=pe)
        dist += sqr(*pi - *(proj++));
      if (dist < 1e-5)
        dist = 1e-5;
      cprob[int(*(proj++))] += exp(exponent2 * log(dist));
    }

    TDiscDistribution *dist = mlnew TDiscDistribution(cprob, nClasses);
    PDistribution wdist = dist;
    dist->normalize();
    return wdist;
  }
  catch (...) {
    delete projection;
    delete cprob;
    throw;
  }

  delete projection;
  delete cprob;

  return PDistribution();
}




TP2NN::TP2NN(PDomain domain, PExampleGenerator egen, PFloatList basesX, PFloatList basesY, const float &e2)
: TPNN(domain, e2)
{ 
  dimensions = 2;
  nExamples = egen->numberOfExamples();

  const int nAttrs = domain->attributes->size();

  if ((basesX->size() != nAttrs) || (basesY->size() != nAttrs))
    raiseError("the number of used attributes, x- and y-anchors coordinates mismatch");

  bases = new double[2*domain->attributes->size()];
  radii = new double[domain->attributes->size()];

  double *bi, *radiii;
  TFloatList::const_iterator bxi(basesX->begin()), bxe(basesX->end());
  TFloatList::const_iterator byi(basesY->begin());
  for(radiii = radii, bi = bases; bxi != bxe; *radiii++ = sqrt(sqr(*bxi) + sqr(*byi)), *bi++ = *bxi++, *bi++ = *byi++);

  const TDomain &gendomain = egen->domain.getReference();
  vector<int> attrIdx;
  attrIdx.reserve(nAttrs);

  offsets = new TFloatList();
  normalizers = new TFloatList();

  if (domain->hasContinuousAttributes()) {
    TDomainBasicAttrStat basstat(egen);
    
    const_PITERATE(TVarList, ai, domain->attributes) {
      const int aidx = gendomain.getVarNum(*ai);
      attrIdx.push_back(aidx);
      if ((*ai)->varType == TValue::INTVAR) {
        offsets->push_back(0);
        normalizers->push_back((*ai)->noOfValues() - 1);
      }
      else if ((*ai)->varType == TValue::FLOATVAR) {
        if (aidx < 0)
          raiseError("P2NN does not accept continuous meta attributes");

        offsets->push_back(basstat[aidx]->min);
        normalizers->push_back(basstat[aidx]->max - basstat[aidx]->min);
      }
      else
        raiseError("P2NN can only handle discrete and continuous attributes");
    }
  }
  else {
    const_PITERATE(TVarList, ai, domain->attributes)
      attrIdx.push_back(gendomain.getVarNum(*ai));
  }

  const int &classIdx = gendomain.getVarNum(domain->classVar);
  
  projections = new double[3*egen->numberOfExamples()];
  double *pi, *pe;
  for(pi = projections, pe = projections + 3*nExamples; pi != pe; *(pi++) = 0.0);

  pi = projections;
  PEITERATE(ei,egen) {
    TFloatList::const_iterator offi(offsets->begin());
    TFloatList::const_iterator nori(normalizers->begin());
    double *base = bases;
    radiii = radii;
    double sumex = 0.0;
    ITERATE(vector<int>, ai, attrIdx) {
      const TValue &val = (*ei)[*ai];
      if (val.isSpecial())
        raiseError("P2NN cannot handle missing values");

      // don't throw out offi if the value is discrete -- we need to increase it
      double aval = ((val.varType == TValue::INTVAR ? float(val.intV) : val.floatV) - *offi++) / *nori++;
      pi[0] += aval * *base++;
      pi[1] += aval * *base++;
      sumex += aval * *radiii++;
    }
    if (sumex > 0.0) {
      pi[0] /= sumex;
      pi[1] /= sumex;
    }

    TValue &cval = (*ei)[classIdx];
    if (cval.isSpecial())
      raiseError("P2NN cannot handle missing class values");
    pi[2] = cval.varType == TValue::INTVAR ? float(cval.intV) : cval.floatV;
    pi += 3;
  }
}


TP2NN::TP2NN(PDomain, double *examples, const int &nEx, double *ba, PFloatList off, PFloatList norm, const float &e2)
: TPNN(domain, e2)
{
  dimensions = 2;
  offsets = off;
  normalizers = norm;

  nExamples = nEx;

  const int nAttrs = domain->attributes->size();
  TFloatList::const_iterator offi, offb = offsets->begin(), offe = offsets->end();
  TFloatList::const_iterator nori, norb = normalizers->begin(), nore = normalizers->end();

  bases = (double *)memcpy(new double[domain->attributes->size()*2], ba, domain->attributes->size()*2*sizeof(double));

  projections = new double[2*nEx];
  double *pi, *pe;
  for(pi = projections, pe = projections + 3*nEx; pi != pe; *(pi++) = 0.0);

  double *example, *examplee;
  for(example = examples, examplee = examples + nExamples*(nAttrs+1), pi = projections; example != examplee; pi += 3) {
    offi = offb;
    nori = norb;
    double *base = bases;
    for(double *ee = example + nAttrs; example != ee; example) {
      double aval = (*(example++) - *(offi++)) / *(nori++);
      pi[0] += aval * *(base++);
      pi[1] += aval * *(base++);
    }
    pi[2] = *example++;
  }
}



void TP2NN::project(const TExample &example, double &x, double &y)
{
  TFloatList::const_iterator offi = offsets->begin(), nori = normalizers->begin();
  x = y = 0.0;
  double *base = bases;

  TExample::const_iterator ei = example.begin();
  for(int attrs = example.domain->attributes->size(); attrs--; ) {
    if ((*ei).isSpecial())
      raiseError("cannot handle missing values");

    double aval = ((*(ei++)).floatV - *(offi++)) / *(nori++);
    x += aval * *(base++);
    y += aval * *(base++);
  }
}


PDistribution TP2NN::classDistribution(const TExample &example)
{
  double x, y;
  const int nClasses = domain->classVar->noOfValues();
  float *cprob = mlnew float[nClasses];
  
  try {
    if (example.domain == domain)
      project(example, x, y);
    else {
      TExample nex(domain, example);
      project(example, x, y);
    }

    classDistribution(x, y, cprob, nClasses);

    TDiscDistribution *dist = mlnew TDiscDistribution(cprob, nClasses);
    PDistribution wdist = dist;
    dist->normalize();
    return wdist;
  }
  catch (...) {
    delete cprob;
    throw;
  }

  return PDistribution();
}


void TP2NN::classDistribution(const double &x, const double &y, float *distribution, const int &nClasses) const
{
  for(float *ci = distribution, *ce = distribution + nClasses; ci != ce; *ci++ = 0.0);
  for(double *proj = projections, *proje = projections + 3*nExamples; proj != proje; proj += 3) {
    double dist = sqr(proj[0] - x) + sqr(proj[1] - y);
    if (dist < 1e-5)
      dist = 1e-5;
    distribution[int(proj[2])] += exp(exponent2 * log(dist));
  }
}
