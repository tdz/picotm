#
# SYNOPSIS
#
#   CONFIG_LIBM
#
# LICENSE
#
#   picotm - A system-level transaction manager
#   Copyright (c) 2017-2018 Thomas Zimmermann <contact@tzimmermann.org>
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Lesser General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public License
#   along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
#   SPDX-License-Identifier: LGPL-3.0-or-later
#

AC_DEFUN([_CHECK_LIBM_COMPLEX_H], [
    AC_CHECK_HEADERS([complex.h])
    AS_VAR_IF([ac_cv_header_complex_h], [yes], [

        #
        # Types
        #

        _CHECK_MODULE_TYPE([libm],
                           [double _Complex],
                           [[@%:@include <complex.h>]])
        _CHECK_MODULE_TYPE([libm],
                           [float _Complex],
                           [[@%:@include <complex.h>]])
        _CHECK_MODULE_TYPE([libm],
                           [long double _Complex],
                           [[@%:@include <complex.h>]])

        #
        # Public interfaces
        #

        _CHECK_MODULE_INTF([libm], [cabs],    [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cabsf],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cabsl],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cacos],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cacosf],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cacosh],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cacoshf], [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cacoshl], [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cacosl],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [carg],    [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cargf],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cargl],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [casin],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [casinf],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [casinh],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [casinhf], [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [casinhl], [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [casinl],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [catan],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [catanf],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [catanh],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [catanhf], [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [catanhl], [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [catanl],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [ccos],    [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [ccosf],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [ccosh],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [ccoshf],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [ccoshl],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [ccosl],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cexp],    [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cexpf],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cexpl],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cimag],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cimagf],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cimagl],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [clog],    [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [clogf],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [clogl],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [conj],    [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [conjf],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [conjl],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cpow],    [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cpowf],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cpowl],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cproj],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cprojf],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [cprojl],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [creal],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [crealf],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [creall],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [csin],    [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [csinf],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [csinh],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [csinhf],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [csinhl],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [csinl],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [csqrt],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [csqrtf],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [csqrtl],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [ctan],    [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [ctanf],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [ctanh],   [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [ctanhf],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [ctanhl],  [[@%:@include <complex.h>]])
        _CHECK_MODULE_INTF([libm], [ctanl],   [[@%:@include <complex.h>]])
    ])
])

AC_DEFUN([_CHECK_LIBM_MATH_H], [
    AC_CHECK_HEADERS([math.h])
    AS_VAR_IF([ac_cv_header_math_h], [yes], [

        #
        # Types
        #

        _CHECK_MODULE_TYPE([libm], [double_t], [[@%:@include <math.h>]])
        _CHECK_MODULE_TYPE([libm], [float_t],  [[@%:@include <math.h>]])

        #
        # Public interfaces
        #

        _CHECK_MODULE_INTF([libm], [acos],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [acosf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [acosh],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [acoshf],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [acoshl],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [acosl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [asin],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [asinf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [asinh],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [asinhf],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [asinhl],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [asinl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [atan],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [atan2],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [atan2f],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [atan2l],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [atanf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [atanh],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [atanhf],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [atanhl],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [atanl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [cbrt],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [cbrtf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [cbrtl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [ceil],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [ceilf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [ceill],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [copysign],    [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [copysignf],   [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [copysignl],   [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [cos],         [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [cosf],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [cosh],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [coshf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [coshl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [cosl],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [erf],         [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [erfc],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [erfcf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [erfcl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [erff],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [erfl],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [exp],         [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [exp2],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [exp2f],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [exp2l],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [expf],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [expl],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [expm1],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [expm1f],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [expm1l],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fabs],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fabsf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fabsl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fdim],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fdimf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fdiml],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [floor],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [floorf],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [floorl],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fma],         [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fmaf],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fmal],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fmax],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fmaxf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fmaxl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fmin],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fminf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fminl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fmod],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fmodf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [fmodl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [frexp],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [frexpf],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [frexpl],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [hypot],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [hypotf],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [hypotl],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [ilogb],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [ilogbf],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [ilogbl],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [j0],          [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [j1],          [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [jn],          [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [ldexp],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [ldexpf],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [ldexpl],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [lgamma],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [lgammaf],     [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [lgammal],     [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [llrint],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [llrintf],     [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [llrintl],     [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [llround],     [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [llroundf],    [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [llroundl],    [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [log],         [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [log10],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [log10f],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [log10l],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [log1p],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [log1pf],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [log1pl],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [log2],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [log2f],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [log2l],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [logb],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [logbf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [logbl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [logf],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [logl],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [lrint],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [lrintf],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [lrintl],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [lround],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [lroundf],     [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [lroundl],     [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [modf],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [modff],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [modfl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [nan],         [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [nanf],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [nanl],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [nearbyint],   [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [nearbyintf],  [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [nearbyintl],  [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [nextafter],   [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [nextafterf],  [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [nextafterl],  [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [nexttoward],  [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [nexttowardf], [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [nexttowardl], [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [pow],         [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [powf],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [powl],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [remainder],   [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [remainderf],  [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [remainderl],  [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [remquo],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [remquof],     [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [remquol],     [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [rint],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [rintf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [rintl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [round],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [roundf],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [roundl],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [scalbln],     [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [scalblnf],    [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [scalblnl],    [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [scalbn],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [scalbnf],     [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [scalbnl],     [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [sin],         [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [sinf],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [sinh],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [sinhf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [sinhl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [sinl],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [sqrt],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [sqrtf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [sqrtl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [tan],         [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [tanf],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [tanh],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [tanhf],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [tanhl],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [tanl],        [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [tgamma],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [tgammaf],     [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [tgammal],     [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [trunc],       [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [truncf],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [truncl],      [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [y0],          [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [y1],          [[@%:@include <math.h>]])
        _CHECK_MODULE_INTF([libm], [yn],          [[@%:@include <math.h>]])
    ])
])

AC_DEFUN([_CONFIG_LIBM], [
    AS_VAR_SET([have_libm], [no])
    AC_CHECK_LIB([m], [signgam])
    AS_VAR_IF([ac_cv_lib_m_signgam], [yes],
              [AS_VAR_SET([have_libm], [yes])])
    # On Cygnus and other systems, signgam is provided by a
    # reentrant function.
    AC_CHECK_LIB([m], [__signgam])
    AS_VAR_IF([ac_cv_lib_m___signgam], [yes],
              [AS_VAR_SET([have_libm], [yes])])
    AS_VAR_IF([have_libm], [yes], [
        _CHECK_LIBM_COMPLEX_H
        _CHECK_LIBM_MATH_H
    ])
])

AC_DEFUN([CONFIG_LIBM], [
    AC_ARG_ENABLE([module-libm],
                  [AS_HELP_STRING([--enable-module-libm],
                                  [enable C Standard Math Library module @<:@default=yes@:>@])],
                  [enable_module_libm=$enableval],
                  [enable_module_libm=yes])
    AM_CONDITIONAL([ENABLE_MODULE_LIBM],
                   [test "x$enable_module_libm" = "xyes"])
    AS_VAR_IF([enable_module_libm], [yes], [
        _CONFIG_LIBM
    ])
])
