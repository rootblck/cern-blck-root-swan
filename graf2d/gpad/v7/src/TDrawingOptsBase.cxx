/// \file TDrawingOptsBase.cxx
/// \ingroup Gpad ROOT7
/// \author Axel Naumann <axel@cern.ch>
/// \date 2017-09-26
/// \warning This is part of the ROOT 7 prototype! It will change without notice. It might trigger earthquakes. Feedback
/// is welcome!

/*************************************************************************
 * Copyright (C) 1995-2017, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "ROOT/TDrawingOptsBase.hxx"

#include "ROOT/TCanvas.hxx"
#include "ROOT/TColor.hxx"
#include "ROOT/TLogger.hxx"
#include "ROOT/TPad.hxx"

#include "TDrawingOptsReader.hxx" // in src/

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <unordered_map>

using namespace ROOT::Experimental;

template <class PRIMITIVE>
TOptsAttrRef<PRIMITIVE>
TDrawingOptsBaseNoDefault::OptsAttrRefArr<PRIMITIVE>::Register(TCanvas &canv, const PRIMITIVE &val)
{
   fRefArray.push_back(canv.GetAttrTable((PRIMITIVE *)nullptr).Register(val));
   return fRefArray.back();
}

template <class PRIMITIVE>
TOptsAttrRef<PRIMITIVE>
TDrawingOptsBaseNoDefault::OptsAttrRefArr<PRIMITIVE>::SameAs(TCanvas &canv, TOptsAttrRef<PRIMITIVE> idx)
{
   canv.GetAttrTable((PRIMITIVE *)nullptr).IncrUse(idx);
   return idx;
}

template <class PRIMITIVE>
TOptsAttrRef<PRIMITIVE>
TDrawingOptsBaseNoDefault::OptsAttrRefArr<PRIMITIVE>::SameAs(TCanvas &canv, const PRIMITIVE &val)
{
   return canv.GetAttrTable((PRIMITIVE *)nullptr).SameAs(val);
}

template <class PRIMITIVE>
void TDrawingOptsBaseNoDefault::OptsAttrRefArr<PRIMITIVE>::Update(TCanvas &canv, TOptsAttrRef<PRIMITIVE> idx,
                                                                  const PRIMITIVE &val)
{
   canv.GetAttrTable((PRIMITIVE *)nullptr).Update(idx, val);
}

template <class PRIMITIVE>
void TDrawingOptsBaseNoDefault::OptsAttrRefArr<PRIMITIVE>::Release(TCanvas &canv)
{
   for (auto idx: fRefArray)
      canv.GetAttrTable((PRIMITIVE *)nullptr).DecrUse(idx);
   fRefArray.clear();
}

template <class PRIMITIVE>
void TDrawingOptsBaseNoDefault::OptsAttrRefArr<PRIMITIVE>::RegisterCopy(TCanvas &canv)
{
   for (auto idx: fRefArray)
      canv.GetAttrTable((PRIMITIVE *)nullptr).IncrUse(idx);
}

template <class PRIMITIVE>
const PRIMITIVE &TDrawingOptsBaseNoDefault::OptsAttrRefArr<PRIMITIVE>::Get(TCanvas &canv, TOptsAttrRef<PRIMITIVE> idx) const
{
   return canv.GetAttrTable((PRIMITIVE *)nullptr).Get(idx);
}

template <class PRIMITIVE>
PRIMITIVE &TDrawingOptsBaseNoDefault::OptsAttrRefArr<PRIMITIVE>::Get(TCanvas &canv, TOptsAttrRef<PRIMITIVE> idx)
{
   return canv.GetAttrTable((PRIMITIVE *)nullptr).Get(idx);
}

template <class PRIMITIVE>
TDrawingOptsBaseNoDefault::OptsAttrRefArr<PRIMITIVE>::~OptsAttrRefArr()
{
   if (!fRefArray.empty())
      R__ERROR_HERE("Gpad") << "Drawing attributes table not empty - must call Release() before!";
}

// Provide specializations promised by the header:
template class TDrawingOptsBaseNoDefault::OptsAttrRefArr<TColor>;
template class TDrawingOptsBaseNoDefault::OptsAttrRefArr<long long>;
template class TDrawingOptsBaseNoDefault::OptsAttrRefArr<double>;

TDrawingOptsBaseNoDefault::TDrawingOptsBaseNoDefault(TPadBase &pad, string_view configPrefix):
fCanvas(&pad.GetCanvas()) {}

ROOT::Experimental::TPadBase &TDrawingOptsBaseNoDefault::GetDefaultCanvas()
{
   static TCanvas sCanv;
   return sCanv;
}

TDrawingOptsBaseNoDefault::~TDrawingOptsBaseNoDefault()
{
   fColorIdx.Release(GetCanvas());
   fIntIdx.Release(GetCanvas());
   fFPIdx.Release(GetCanvas());
}

TDrawingOptsBaseNoDefault::TDrawingOptsBaseNoDefault(const TDrawingOptsBaseNoDefault &other)
   : fCanvas(other.fCanvas), fColorIdx(other.fColorIdx), fIntIdx(other.fIntIdx), fFPIdx(other.fFPIdx)
{
   fColorIdx.RegisterCopy(GetCanvas());
   fIntIdx.RegisterCopy(GetCanvas());
   fFPIdx.RegisterCopy(GetCanvas());
}
