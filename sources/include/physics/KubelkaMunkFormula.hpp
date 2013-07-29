/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _KUBELKAMUNKFORMULA_HPP
#define _KUBELKAMUNKFORMULA_HPP

class KubelkaMunkFormula{
public : 
  /**
   * Compute the reflectance of a pigmented layer.
   * @param K : the K parameter of the layer.
   * @param S : the S parameter of the layer.
   * @param thickness : the thickness of the layer (in micrometer)
   * @param Rsub : the reflectance of the substrate.
   * @return : the computed reflectance.
   */
  inline static Real kublekaMunkReflectance(const Real& K, const Real& S, const Real& thickness, const Real& Rsub);

  /**
   * Compute the reflectance of an infinite pigmented layer.
   * @param K : the K parameter of the layer.
   * @param S : the S parameter of the layer.
   * @return : the computed reflectance.
   */
  inline static Real kublekaMunkInfiniteReflectance(const Real& K, const Real& S);

  inline static Real kubelkaMunkDirectionnalAbsorptionCoeficient(const Real& K, const Real& S);
};

/**
 * Compute the reflectance of a pigmented layer.
 * @param K : the K parameter of the layer.
 * @param S : the S parameter of the layer.
 * @param thickness : the thickness of the layer (in micrometer)
 * @param Rsub : the reflectance of the substrate.
 * @return : the computed reflectance.
 */
inline Real KubelkaMunkFormula::kublekaMunkReflectance(const Real& K, const Real& S, const Real& thickness, const Real& Rsub){
  Real Rinf = kublekaMunkInfiniteReflectance(K, S);
  if(Rinf <= 0.001)
    return Rsub*std::exp(-2*thickness*K);

  Real e = std::exp(S*thickness *(1/Rinf - Rinf));
  return   ((1/Rinf)*(Rsub-Rinf) - Rinf*(Rsub- 1/Rinf)*e)
         / ((Rsub-Rinf)-(Rsub- 1/Rinf)*e);
}

/**
 * Compute the reflectance of an infinite pigmented layer.
 * @param K : the K parameter of the layer.
 * @param S : the S parameter of the layer.
 * @return : the computed reflectance.
 */
inline Real KubelkaMunkFormula::kublekaMunkInfiniteReflectance(const Real& K, const Real& S){
  if(S<=0.0001 || K<0.0)
    return 0;

  Real KS = K/S;
  return 1 + KS - std::sqrt(KS*KS + 2 * KS);
}

inline Real KubelkaMunkFormula::kubelkaMunkDirectionnalAbsorptionCoeficient(const Real& K, const Real& S)
{
  return K+S;
  /*
    Real Rsub = 0.5;
    Real Rinf = kublekaMunkInfiniteReflectance(K,S);
    Real R    = kublekaMunkReflectance(K, S, 1.0, Rsub);
    Real Rbk   = (Rinf*(Rsub - R))
              / (Rsub - Rinf*(1-Rsub*Rinf + Rsub*R));
    Real T    = std::sqrt((R - Rbk)/Rsub);

    return std::log(1/T);
  */
}



#endif // _KUBELKAMUNKFORMULA_HPP

