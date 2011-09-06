/*
   Copyright (c) 2009-2011, Jack Poulson
   All rights reserved.

   This file is part of Elemental.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

    - Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    - Neither the name of the owner nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef ELEMENTAL_ENVIRONMENT_HPP
#define ELEMENTAL_ENVIRONMENT_HPP 1

#include "mpi.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <vector>

#include "elemental/config.h"

// If defined, the _OPENMP macro contains the date of the specification
#ifdef _OPENMP
# include <omp.h>
# if _OPENMP >= 200805
#  define COLLAPSE(N) collapse(N)
# else
#  define COLLAPSE(N) 
# endif 
#endif

#ifndef RELEASE
namespace elemental {

void PushCallStack( std::string s );
void PopCallStack();
void DumpCallStack();

}
#endif

#include "elemental/types.hpp"
#include "elemental/imports.hpp"
#include "elemental/utilities.hpp"

#include "elemental/memory.hpp"
#include "elemental/grid.hpp"
#include "elemental/random.hpp"
#include "elemental/timer.hpp"

// Template conventions:
//   G: general datatype
//
//   T: any ring, e.g., the (Gaussian) integers and the real/complex numbers
//   Z: representation of a real ring, e.g., the integers or real numbers
//   std::complex<Z>: representation of a complex ring, e.g. Gaussian integers
//                    or complex numbers
//
//   F: representation of real or complex number
//   R: representation of real number
//   std::complex<R>: representation of complex number

namespace elemental {

void Init( int& argc, char**& argv );
void Finalize();

// Naive blocksize set and get
int Blocksize();
void SetBlocksize( int blocksize );

void PushBlocksizeStack( int blocksize );
void PopBlocksizeStack();

template<typename Z>
Z Abs( Z alpha );

#ifndef WITHOUT_COMPLEX
template<typename Z>
Z Abs( std::complex<Z> alpha );
#endif

template<typename Z>
Z FastAbs( Z alpha );

#ifndef WITHOUT_COMPLEX
template<typename Z>
Z FastAbs( std::complex<Z> alpha );
#endif

template<typename Z>
Z Conj( Z alpha );

#ifndef WITHOUT_COMPLEX
template<typename Z>
std::complex<Z> Conj( std::complex<Z> alpha );
#endif

// For extracting the underlying real datatype, 
// e.g., typename RealBase<Scalar>::type a = 3.0;
template<typename R>
struct RealBase
{ typedef R type; };

template<typename R>
struct RealBase<std::complex<R> >
{ typedef R type; };

// We define an output stream that does nothing. This is done so that the 
// root process can be used to print data to a file's ostream while all other 
// processes use a null ostream. This is used within the DistMatrix class's
// 'Write' functions.
struct NullStream : std::ostream
{
    struct NullStreamBuffer : std::streambuf
    {
        int overflow( int c ) { return traits_type::not_eof(c); }
    } _nullStreamBuffer;

    NullStream() 
    : std::ios(&_nullStreamBuffer), std::ostream(&_nullStreamBuffer) 
    { }
};

// Create a wrappers around real and std::complex<real> types so that they
// can be conveniently printed in a more Matlab-compatible format.
//
// All printing of scalars should now be performed in the fashion:
//     std::cout << WrapScalar(alpha);
// where 'alpha' can be real or complex.

template<typename R>
class ScalarWrapper
{
    const R _value;
public:
    ScalarWrapper( R alpha ) : _value(alpha) { }

    friend std::ostream& operator<<
    ( std::ostream& out, ScalarWrapper<R> alpha )
    {
        out << alpha._value;
        return out;
    }
};

#ifndef WITHOUT_COMPLEX
template<typename R>
class ScalarWrapper<std::complex<R> >
{
    const std::complex<R> _value;
public:
    ScalarWrapper( std::complex<R> alpha ) : _value(alpha) { }

    friend std::ostream& operator<<
    ( std::ostream& os, ScalarWrapper<std::complex<R> > alpha )
    {
        os << std::real(alpha._value) << "+" << std::imag(alpha._value) << "i";
        return os;
    }
};
#endif // ifndef WITHOUT_COMPLEX

// There is a known bug in the Darwin g++ that causes an internal compiler
// error, so, by default, this routine is subverted.
#ifdef DISABLE_SCALAR_WRAPPER

template<typename R>
R WrapScalar( R alpha );
#ifndef WITHOUT_COMPLEX
template<typename R>
std::complex<R> WrapScalar( std::complex<R> alpha );
#endif // ifndef WITHOUT_COMPLEX

#else  // ifdef DISABLE_SCALAR_WRAPPER

template<typename R>
ScalarWrapper<R> WrapScalar( R alpha );
#ifndef WITHOUT_COMPLEX
template<typename R>
ScalarWrapper<std::complex<R> > WrapScalar( std::complex<R> alpha );
#endif // ifndef WITHOUT_COMPLEX

#endif // ifdef DISABLE_SCALAR_WRAPPER

} // elemental

//----------------------------------------------------------------------------//
// Implementation begins here                                                 //
//----------------------------------------------------------------------------//

namespace elemental {

#ifdef DISABLE_SCALAR_WRAPPER

template<typename R>
inline R
WrapScalar( R alpha )
{ return alpha; }
#ifndef WITHOUT_COMPLEX
template<typename R>
inline std::complex<R>
WrapScalar( std::complex<R> alpha )
{ return alpha; }
#endif // ifndef WITHOUT_COMPLEX

#else // ifdef DISABLE_SCALAR_WRAPPER

template<typename R>
inline ScalarWrapper<R>
WrapScalar( R alpha )
{ return ScalarWrapper<R>( alpha ); }
#ifndef WITHOUT_COMPLEX
template<typename R>
inline ScalarWrapper<std::complex<R> >
WrapScalar( std::complex<R> alpha )
{ return ScalarWrapper<std::complex<R> >( alpha ); }
#endif // ifndef WITHOUT_COMPLEX

#endif // ifdef DISABLE_SCALAR_WRAPPER

template<typename Z>
inline Z 
Abs( Z alpha )
{ return std::abs(alpha); }

#ifndef WITHOUT_COMPLEX
template<typename Z>
inline Z
Abs( std::complex<Z> alpha )
{ return std::abs( alpha ); }
#endif

template<typename Z>
inline Z
FastAbs( Z alpha )
{ return std::abs(alpha); }

#ifndef WITHOUT_COMPLEX
template<typename Z>
inline Z
FastAbs( std::complex<Z> alpha )
{ return std::abs( std::real(alpha) ) + std::abs( std::imag(alpha) ); }
#endif

template<typename Z>
inline Z
Conj
( Z alpha )
{ return alpha; }

#ifndef WITHOUT_COMPLEX
template<typename Z>
inline std::complex<Z>
Conj( std::complex<Z> alpha )
{ return std::conj( alpha ); }
#endif

} // namespace elemental

#endif /* ELEMENTAL_ENVIRONMENT_HPP */

