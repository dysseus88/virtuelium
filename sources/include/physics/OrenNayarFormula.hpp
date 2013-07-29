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

#ifndef _ORENNAYARFORMULA_HPP
#define _ORENNAYARFORMULA_HPP

class OrenNayarFormula{
public :
  static inline Real orenNayarReflectance(Real Oi, Real Pi, Real Ov, Real Pv, Real roughness);
};

inline Real OrenNayarFormula::orenNayarReflectance(Real Oi, Real Pi, Real Ov, Real Pv, Real roughness)
{
  Real A = 1 - 0.5 * (roughness*roughness)/(roughness*roughness + 0.33);
  Real B = 0.45 * (roughness*roughness)/(roughness*roughness + 0.09);

  Real a = (Oi > Ov)? Oi : Ov;
  Real b = (Oi < Ov)? Oi : Ov;
  Real c = std::cos(Pi - Pv);
  if(c<0) c=0;

  return std::cos(Oi)*(A + B * c * std::sin(a) * std::tan(b)) / M_PI;
}

#endif //_ORENNAYARFORMULA_HPP

