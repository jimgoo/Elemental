/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/

namespace elem {

template<typename T,typename Int>
inline
DistMatrix<T,STAR,MD,Int>::DistMatrix( const elem::Grid& g )
: AbstractDistMatrix<T,Int>
  (0,0,false,false,0,0,
   0,(g.InGrid() && g.DiagPath()==0 ? g.DiagPathRank() : 0),
   0,0,g),
  diagPath_(0)
{ }

template<typename T,typename Int>
inline
DistMatrix<T,STAR,MD,Int>::DistMatrix
( Int height, Int width, const elem::Grid& g )
: AbstractDistMatrix<T,Int>
  (height,width,false,false,0,0,
   0,
   (g.InGrid() && g.DiagPath()==0 ? g.DiagPathRank() : 0),height,
   (g.InGrid() && g.DiagPath()==0 ? 
    LocalLength(width,g.DiagPathRank(),0,g.LCM()) : 0),g),
  diagPath_(0)
{ }

template<typename T,typename Int>
inline
DistMatrix<T,STAR,MD,Int>::DistMatrix
( bool constrainedRowAlignment, Int rowAlignmentVC, const elem::Grid& g )
: AbstractDistMatrix<T,Int>
  (0,0,false,constrainedRowAlignment,0,g.DiagPathRank(rowAlignmentVC),
   0,
   (g.InGrid() && g.DiagPath()==g.DiagPath(rowAlignmentVC) ?
    Shift(g.DiagPathRank(),g.DiagPathRank(rowAlignmentVC),g.LCM()) : 0),
   0,0,g),
  diagPath_(g.DiagPath(rowAlignmentVC))
{ }

template<typename T,typename Int>
inline
DistMatrix<T,STAR,MD,Int>::DistMatrix
( Int height, Int width, bool constrainedRowAlignment, Int rowAlignmentVC,
  const elem::Grid& g )
: AbstractDistMatrix<T,Int>
  (height,width,false,constrainedRowAlignment,0,g.DiagPathRank(rowAlignmentVC),
   0,
   (g.InGrid() && g.DiagPath()==g.DiagPath(rowAlignmentVC) ?
    Shift(g.DiagPathRank(),g.DiagPathRank(rowAlignmentVC),g.LCM()) : 0),
   height,
   (g.InGrid() && g.DiagPath()==g.DiagPath(rowAlignmentVC) ?
    LocalLength(width,g.DiagPathRank(),g.DiagPathRank(rowAlignmentVC),g.LCM()) :
    0),g),
  diagPath_(g.DiagPath(rowAlignmentVC))
{ }

template<typename T,typename Int>
inline
DistMatrix<T,STAR,MD,Int>::DistMatrix
( Int height, Int width, bool constrainedRowAlignment, Int rowAlignmentVC,
  Int ldim, const elem::Grid& g )
: AbstractDistMatrix<T,Int>
  (height,width,false,constrainedRowAlignment,0,g.DiagPathRank(rowAlignmentVC),
   0,
   (g.InGrid() && g.DiagPath()==g.DiagPath(rowAlignmentVC) ?
    Shift(g.DiagPathRank(),g.DiagPathRank(rowAlignmentVC),g.LCM()) : 0),
   height,
   (g.InGrid() && g.DiagPath()==g.DiagPath(rowAlignmentVC) ?
    LocalLength(width,g.DiagPathRank(),g.DiagPathRank(rowAlignmentVC),g.LCM()) :
    0),ldim,g),
  diagPath_(g.DiagPath(rowAlignmentVC))
{ }

template<typename T,typename Int>
inline
DistMatrix<T,STAR,MD,Int>::DistMatrix
( Int height, Int width, Int rowAlignmentVC, const T* buffer, Int ldim,
  const elem::Grid& g )
: AbstractDistMatrix<T,Int>
  (height,width,0,g.DiagPathRank(rowAlignmentVC),
   0,
   (g.InGrid() && g.DiagPath()==g.DiagPath(rowAlignmentVC) ?
    Shift(g.DiagPathRank(),g.DiagPathRank(rowAlignmentVC),g.LCM()) : 0),
   height,
   (g.InGrid() && g.DiagPath()==g.DiagPath(rowAlignmentVC) ?
    LocalLength(width,g.DiagPathRank(),g.DiagPathRank(rowAlignmentVC),g.LCM()) :
    0),buffer,ldim,g),
  diagPath_(g.DiagPath(rowAlignmentVC))
{ }

template<typename T,typename Int>
inline
DistMatrix<T,STAR,MD,Int>::DistMatrix
( Int height, Int width, Int rowAlignmentVC, T* buffer, Int ldim,
  const elem::Grid& g )
: AbstractDistMatrix<T,Int>
  (height,width,0,g.DiagPathRank(rowAlignmentVC),
   0,
   (g.InGrid() && g.DiagPath()==g.DiagPath(rowAlignmentVC) ?
    Shift(g.DiagPathRank(),g.DiagPathRank(rowAlignmentVC),g.LCM()) : 0),
   height,
   (g.InGrid() && g.DiagPath()==g.DiagPath(rowAlignmentVC) ?
    LocalLength(width,g.DiagPathRank(),g.DiagPathRank(rowAlignmentVC),g.LCM()) :
    0),buffer,ldim,g),
  diagPath_(g.DiagPath(rowAlignmentVC))
{ }

template<typename T,typename Int>
template<Distribution U,Distribution V>
inline
DistMatrix<T,STAR,MD,Int>::DistMatrix( const DistMatrix<T,U,V,Int>& A )
: AbstractDistMatrix<T,Int>(0,0,false,false,0,0,
  0,(A.Participating() ? A.RowRank() : 0),
  0,0,A.Grid()),
  diagPath_(A.diagPath_)
{
#ifndef RELEASE
    PushCallStack("DistMatrix[* ,MD]::DistMatrix");
#endif
    if( STAR != U || MD != V || 
        reinterpret_cast<const DistMatrix<T,STAR,MD,Int>*>(&A) != this )
        *this = A;
    else
        throw std::logic_error("Tried to construct [* ,MD] with itself");
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T,typename Int>
inline
DistMatrix<T,STAR,MD,Int>::~DistMatrix()
{ }

template<typename T,typename Int>
inline void
DistMatrix<T,STAR,MD,Int>::SetGrid( const elem::Grid& g )
{
    this->Empty();
    this->grid_ = &g;
    this->diagPath_ = 0;
    this->rowAlignment_ = 0;
    if( g.InGrid() && g.DiagPath()==0 )
        this->rowShift_ = g.DiagPathRank();
    else
        this->rowShift_ = 0;
}

template<typename T,typename Int>
inline Int
DistMatrix<T,STAR,MD,Int>::ColStride() const
{ return 1; }

template<typename T,typename Int>
inline Int
DistMatrix<T,STAR,MD,Int>::RowStride() const
{ return this->grid_->LCM(); }

template<typename T,typename Int>
inline Int
DistMatrix<T,STAR,MD,Int>::ColRank() const
{ return 0; }

template<typename T,typename Int>
inline Int
DistMatrix<T,STAR,MD,Int>::RowRank() const
{ return this->grid_->DiagPathRank(); }

template<typename T,typename Int>
inline bool
DistMatrix<T,STAR,MD,Int>::Participating() const
{
    const Grid& g = this->Grid();
    return ( g.InGrid() && g.DiagPath()==this->diagPath_ );
}

template<typename T,typename Int>
inline Int
DistMatrix<T,STAR,MD,Int>::DiagPath() const
{ return this->diagPath_; }

template<typename T,typename Int>
template<typename S,typename N>
inline void
DistMatrix<T,STAR,MD,Int>::AlignWith( const DistMatrix<S,STAR,MD,N>& A )
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::AlignWith([* ,MD])");
    this->AssertFreeRowAlignment();
    this->AssertSameGrid( A );
#endif
    this->Empty();
    this->diagPath_ = A.diagPath_;
    this->rowAlignment_ = A.rowAlignment_;
    this->constrainedRowAlignment_ = true;
    this->rowShift_ = A.RowShift();
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T,typename Int>
template<typename S,typename N>
inline void
DistMatrix<T,STAR,MD,Int>::AlignWith( const DistMatrix<S,MD,STAR,N>& A )
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::AlignWith([MD,* ])");
    this->AssertFreeRowAlignment();
    this->AssertSameGrid( A );
#endif
    this->Empty();
    this->diagPath_ = A.diagPath_;
    this->rowAlignment_ = A.colAlignment_;
    this->constrainedRowAlignment_ = true;
    this->rowShift_ = A.ColShift();
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T,typename Int>
template<typename S,typename N>
inline void
DistMatrix<T,STAR,MD,Int>::AlignRowsWith( const DistMatrix<S,STAR,MD,N>& A )
{ AlignWith( A ); }

template<typename T,typename Int>
template<typename S,typename N>
inline void
DistMatrix<T,STAR,MD,Int>::AlignRowsWith( const DistMatrix<S,MD,STAR,N>& A )
{ AlignWith( A ); }

template<typename T,typename Int>
template<typename S,typename N>
inline bool
DistMatrix<T,STAR,MD,Int>::AlignedWithDiagonal
( const DistMatrix<S,MC,MR,N>& A, Int offset ) const
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::AlignedWithDiagonal([MC,MR])");
    this->AssertSameGrid( A );
#endif
    const elem::Grid& g = this->Grid();
    const Int r = g.Height();
    const Int c = g.Width();
    const Int colAlignment = A.ColAlignment();
    const Int rowAlignment = A.RowAlignment();

    const Int firstDiagRow = 0;
    const Int firstDiagCol = this->diagPath_;
    const Int diagRow = (firstDiagRow+this->RowAlignment()) % r;
    const Int diagCol = (firstDiagCol+this->RowAlignment()) % c;

    bool aligned;
    if( offset >= 0 )
    {
        const Int ownerRow = colAlignment;
        const Int ownerCol = (rowAlignment + offset) % c;
        aligned = ( ownerRow==diagRow && ownerCol==diagCol );
    }
    else
    {
        const Int ownerRow = (colAlignment-offset) % r;
        const Int ownerCol = rowAlignment;
        aligned = ( ownerRow==diagRow && ownerCol==diagCol );
    }
#ifndef RELEASE
    PopCallStack();
#endif
    return aligned;
}

template<typename T,typename Int>
template<typename S,typename N>
inline bool
DistMatrix<T,STAR,MD,Int>::AlignedWithDiagonal
( const DistMatrix<S,MR,MC,N>& A, Int offset ) const
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::AlignedWithDiagonal([MR,MC])");
    this->AssertSameGrid( A );
#endif
    const elem::Grid& g = this->Grid();
    const Int r = g.Height();
    const Int c = g.Width();
    const Int colAlignment = A.ColAlignment();
    const Int rowAlignment = A.RowAlignment();

    const Int firstDiagRow = 0;
    const Int firstDiagCol = this->diagPath_;
    const Int diagRow = (firstDiagRow+this->RowAlignment()) % r;
    const Int diagCol = (firstDiagCol+this->RowAlignment()) % c;

    bool aligned;
    if( offset >= 0 )
    {
        const Int ownerRow = rowAlignment;
        const Int ownerCol = (colAlignment + offset) % c;
        aligned = ( ownerRow==diagRow && ownerCol==diagCol );
    }
    else
    {
        const Int ownerRow = (rowAlignment-offset) % r;
        const Int ownerCol = colAlignment;
        aligned = ( ownerRow==diagRow && ownerCol==diagCol );
    }
#ifndef RELEASE
    PopCallStack();
#endif
    return aligned;
}

template<typename T,typename Int>
template<typename S,typename N>
inline void
DistMatrix<T,STAR,MD,Int>::AlignWithDiagonal
( const DistMatrix<S,MC,MR,N>& A, Int offset )
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::AlignWithDiagonal([MC,MR])");
    this->AssertFreeRowAlignment();
    this->AssertSameGrid( A );
#endif
    const elem::Grid& g = this->Grid();
    const Int r = g.Height();
    const Int c = g.Width();
    const Int lcm = g.LCM();
    const Int colAlignment = A.ColAlignment();
    const Int rowAlignment = A.RowAlignment();

    this->Empty();
    Int owner;
    if( offset >= 0 )
    {
        const Int ownerRow = colAlignment;
        const Int ownerCol = (rowAlignment + offset) % c;
        owner = ownerRow + r*ownerCol;
    } 
    else
    {
        const Int ownerRow = (colAlignment-offset) % r;
        const Int ownerCol = rowAlignment;
        owner = ownerRow + r*ownerCol;
    }
    this->diagPath_ = g.DiagPath(owner);
    this->rowAlignment_ = g.DiagPathRank(owner);
    this->constrainedRowAlignment_ = true;
    if( this->Participating() )
        this->rowShift_ = (g.DiagPathRank()+lcm-this->rowAlignment_) % lcm;
    else
        this->rowShift_ = 0;
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T,typename Int>
template<typename S,typename N>
inline void
DistMatrix<T,STAR,MD,Int>::AlignWithDiagonal
( const DistMatrix<S,MR,MC,N>& A, Int offset )
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::AlignWithDiagonal([MR,MC])");
    this->AssertFreeRowAlignment();
    this->AssertSameGrid( A );
#endif
    const elem::Grid& g = this->Grid();
    const Int r = g.Height();
    const Int c = g.Width();
    const Int lcm = g.LCM();
    const Int colAlignment = A.ColAlignment();
    const Int rowAlignment = A.RowAlignment();

    this->Empty();
    Int owner;
    if( offset >= 0 )
    {
        const Int ownerRow = rowAlignment;
        const Int ownerCol = (colAlignment + offset) % c;
        owner = ownerRow + r*ownerCol;
    }
    else
    {
        const Int ownerRow = (rowAlignment-offset) % r;
        const Int ownerCol = colAlignment;
        owner = ownerRow + r*ownerCol;
    }
    this->diagPath_ = g.DiagPath(owner);
    this->rowAlignment_ = g.DiagPathRank(owner);
    this->constrainedRowAlignment_ = true;
    if( this->Participating() )
        this->rowShift_ = (g.DiagPathRank()+lcm-this->rowAlignment_) % lcm;
    else
        this->rowShift_ = 0;
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T,typename Int>
inline void
DistMatrix<T,STAR,MD,Int>::PrintBase
( std::ostream& os, const std::string msg ) const
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::PrintBase");
#endif
    if( this->Grid().Rank() == 0 && msg != "" )
        os << msg << std::endl;
        
    const Int height     = this->Height();
    const Int width      = this->Width();
    const Int localWidth = this->LocalWidth();
    const Int lcm        = this->Grid().LCM();

    if( height == 0 || width == 0 || !this->Grid().InGrid() )
    {
#ifndef RELEASE
        PopCallStack();
#endif
        return;
    }

    std::vector<T> sendBuf(height*width,0);
    if( this->Participating() )
    {
        const Int colShift = this->ColShift();
        const T* thisLocalBuffer = this->LockedLocalBuffer();
        const Int thisLDim = this->LocalLDim();
#ifdef HAVE_OPENMP
        #pragma omp parallel for
#endif
        for( Int jLocal=0; jLocal<localWidth; ++jLocal )
        {
            T* destCol = &sendBuf[colShift+jLocal*lcm*height];
            const T* sourceCol = &thisLocalBuffer[jLocal*thisLDim];
            for( Int i=0; i<height; ++i )
                destCol[i] = sourceCol[i];
        }
    }

    // If we are the root, allocate a receive buffer
    std::vector<T> recvBuf;
    if( this->Grid().Rank() == 0 )
        recvBuf.resize( height*width );

    // Sum the contributions and send to the root
    mpi::Reduce
    ( &sendBuf[0], &recvBuf[0], height*width, mpi::SUM, 0, 
      this->Grid().Comm() );

    if( this->Grid().Rank() == 0 )
    {
        // Print the data
        for( Int i=0; i<height; ++i )
        {
            for( Int j=0; j<width; ++j )
                os << recvBuf[i+j*height] << " ";
            os << "\n";
        }
        os << std::endl;
    }
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T,typename Int>
inline void
DistMatrix<T,STAR,MD,Int>::Align( Int rowAlignmentVC )
{
#ifndef RELEASE
    PushCallStack("[STAR,MD]::Align");
    this->AssertFreeRowAlignment();
#endif
    this->AlignRows( rowAlignmentVC );
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T,typename Int>
inline void
DistMatrix<T,STAR,MD,Int>::AlignRows( Int rowAlignmentVC )
{
#ifndef RELEASE
    PushCallStack("[STAR,MD]::AlignRows");
    this->AssertFreeRowAlignment();
#endif
    const elem::Grid& g = this->Grid();
    this->Empty();
#ifndef RELEASE
    if( rowAlignmentVC < 0 || rowAlignmentVC >= g.Size() )
        throw std::runtime_error("Invalid row alignment for [STAR,MD]");
#endif
    this->diagPath_ = g.DiagPath(rowAlignmentVC);
    this->rowAlignment_ = g.DiagPathRank(rowAlignmentVC);
    this->constrainedRowAlignment_ = true;
    if( this->Participating() )
        this->rowShift_ = 
            Shift( g.DiagPathRank(), this->rowAlignment_, g.LCM() );
    else
        this->rowShift_ = 0;
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T,typename Int>
inline void
DistMatrix<T,STAR,MD,Int>::Attach
( Int height, Int width, Int rowAlignmentVC,
  T* buffer, Int ldim, const elem::Grid& grid )
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::Attach");
#endif
    this->Empty();

    this->grid_ = &grid;
    this->height_ = height;
    this->width_ = width;
    this->diagPath_ = grid.DiagPath(rowAlignmentVC);
    this->rowAlignment_ = grid.DiagPathRank(rowAlignmentVC);
    this->viewing_ = true;
    if( this->Participating() )
    {
        this->rowShift_ =
            Shift(grid.DiagPathRank(),this->rowAlignment_,grid.LCM());
        const Int localWidth = LocalLength(width,this->rowShift_,grid.LCM());
        this->localMatrix_.Attach( height, localWidth, buffer, ldim );
    }
    else
        this->rowShift_ = 0;
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T,typename Int>
inline void
DistMatrix<T,STAR,MD,Int>::LockedAttach
( Int height, Int width, Int rowAlignmentVC,
  const T* buffer, Int ldim, const elem::Grid& grid )
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::LockedAttach");
#endif
    this->Empty();

    this->grid_ = &grid;
    this->height_ = height;
    this->width_ = width;
    this->diagPath_ = grid.DiagPath(rowAlignmentVC);
    this->rowAlignment_ = grid.DiagPathRank(rowAlignmentVC);
    this->viewing_ = true;
    this->lockedView_ = true;
    if( this->Participating() )
    {
        this->rowShift_ =
            Shift(grid.DiagPathRank(),this->rowAlignment_,grid.LCM());
        const Int localWidth = LocalLength(width,this->rowShift_,grid.LCM());
        this->localMatrix_.LockedAttach( height, localWidth, buffer, ldim );
    }
    else
        this->rowShift_ = 0;
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T,typename Int>
inline void
DistMatrix<T,STAR,MD,Int>::ResizeTo( Int height, Int width )
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::ResizeTo");
    this->AssertNotLockedView();
    if( height < 0 || width < 0 )
        throw std::logic_error("Height and width must be non-negative");
#endif
    this->height_ = height;
    this->width_ = width;
    if( this->Participating() )
    {
        const Int lcm = this->Grid().LCM();
        this->localMatrix_.ResizeTo
        ( height, LocalLength(width,this->RowShift(),lcm) );
    }
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T,typename Int>
inline T
DistMatrix<T,STAR,MD,Int>::Get( Int i, Int j ) const
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::Get");
    this->AssertValidEntry( i, j );
#endif
    // We will determine the owner of entry (i,j) and broadcast from it
    const elem::Grid& g = this->Grid();
    const Int r = g.Height();
    const Int c = g.Width();
    const Int ownerRow = (j + this->rowAlignment_) % r;
    const Int ownerCol = (j + this->rowAlignment_ + this->diagPath_) % c;
    const Int ownerRank = ownerRow + r*ownerCol;

    T u;
    if( g.VCRank() == ownerRank )
    {
        const Int jLoc = (j-this->RowShift()) / g.LCM();
        u = this->GetLocal(i,jLoc);
    }
    mpi::Broadcast( &u, 1, g.VCToViewingMap(ownerRank), g.ViewingComm() );
#ifndef RELEASE
    PopCallStack();
#endif
    return u;
}

template<typename T,typename Int>
inline void
DistMatrix<T,STAR,MD,Int>::Set( Int i, Int j, T u )
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::Set");
    this->AssertValidEntry( i, j );
#endif
    const elem::Grid& g = this->Grid();
    const Int r = g.Height();
    const Int c = g.Width();
    const Int ownerRow = (j + this->rowAlignment_) % r;
    const Int ownerCol = (j + this->rowAlignment_ + this->diagPath_) % c;
    const Int ownerRank = ownerRow + r*ownerCol;

    if( g.VCRank() == ownerRank )
    {
        const Int jLoc = (j-this->RowShift()) / g.LCM();
        this->SetLocal(i,jLoc,u);
    }
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T,typename Int>
inline void
DistMatrix<T,STAR,MD,Int>::Update( Int i, Int j, T u )
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::Update");
    this->AssertValidEntry( i, j );
#endif
    const elem::Grid& g = this->Grid();
    const Int r = g.Height();
    const Int c = g.Width();
    const Int ownerRow = (j + this->rowAlignment_) % r;
    const Int ownerCol = (j + this->rowAlignment_ + this->diagPath_) % c;
    const Int ownerRank = ownerRow + r*ownerCol;

    if( g.VCRank() == ownerRank )
    {
        const Int jLoc = (j-this->RowShift()) / g.LCM();
        this->UpdateLocal(i,jLoc,u);
    }
#ifndef RELEASE
    PopCallStack();
#endif
}

//
// Utility functions, e.g., operator=
//

template<typename T,typename Int>
inline const DistMatrix<T,STAR,MD,Int>&
DistMatrix<T,STAR,MD,Int>::operator=( const DistMatrix<T,MC,MR,Int>& A )
{
#ifndef RELEASE
    PushCallStack("[* ,MD] = [MC,MR]");
    this->AssertNotLockedView();
    this->AssertSameGrid( A );
    if( this->Viewing() )
        this->AssertSameSize( A );
#endif
    throw std::logic_error("[* ,MD] = [MC,MR] not yet implemented");
#ifndef RELEASE
    PopCallStack();
#endif
    return *this;
}

template<typename T,typename Int>
inline const DistMatrix<T,STAR,MD,Int>&
DistMatrix<T,STAR,MD,Int>::operator=( const DistMatrix<T,MC,STAR,Int>& A )
{
#ifndef RELEASE
    PushCallStack("[* ,MD] = [MC,* ]");
    this->AssertNotLockedView();
    this->AssertSameGrid( A );
    if( this->Viewing() )
        this->AssertSameSize( A );
#endif
    throw std::logic_error("[* ,MD] = [MC,* ] not yet implemented");
#ifndef RELEASE
    PopCallStack();
#endif
    return *this;
}

template<typename T,typename Int>
inline const DistMatrix<T,STAR,MD,Int>&
DistMatrix<T,STAR,MD,Int>::operator=( const DistMatrix<T,STAR,MR,Int>& A )
{
#ifndef RELEASE
    PushCallStack("[* ,MD] = [* ,MR]");
    this->AssertNotLockedView();
    this->AssertSameGrid( A );
    if( this->Viewing() )
        this->AssertSameSize( A );
#endif
    throw std::logic_error("[* ,MD] = [* ,MR] not yet implemented");
#ifndef RELEASE
    PopCallStack();
#endif
    return *this;
}

template<typename T,typename Int>
inline const DistMatrix<T,STAR,MD,Int>&
DistMatrix<T,STAR,MD,Int>::operator=( const DistMatrix<T,MD,STAR,Int>& A )
{
#ifndef RELEASE
    PushCallStack("[* ,MD] = [MD,* ]");
    this->AssertNotLockedView();
    this->AssertSameGrid( A );
    if( this->Viewing() )
        this->AssertSameSize( A );
#endif
    throw std::logic_error("[* ,MD] = [MD,* ] not yet implemented");
#ifndef RELEASE
    PopCallStack();
#endif
    return *this;
}

template<typename T,typename Int>
inline const DistMatrix<T,STAR,MD,Int>&
DistMatrix<T,STAR,MD,Int>::operator=( const DistMatrix<T,STAR,MD,Int>& A )
{
#ifndef RELEASE
    PushCallStack("[* ,MD] = [* ,MD]");
    this->AssertNotLockedView();
    this->AssertSameGrid( A );
    if( this->Viewing() )
        this->AssertSameSize( A );
#endif
    if( !this->Viewing() )
    {
        if( !this->ConstrainedRowAlignment() )
        {
            this->diagPath_ = A.diagPath_;
            this->rowAlignment_ = A.rowAlignment_;
            if( this->Participating() )
                this->rowShift_ = A.RowShift();
        }
        this->ResizeTo( A.Height(), A.Width() );
    }

    if( this->diagPath_ == A.diagPath_ && 
        this->rowAlignment_ == A.rowAlignment_ )
    {
        this->localMatrix_ = A.LockedLocalMatrix();
    }
    else
    {
#ifdef UNALIGNED_WARNINGS
        if( this->Grid().Rank() == 0 )
            std::cerr << "Unaligned [* ,MD] <- [* ,MD]." << std::endl;
#endif
        throw std::logic_error
        ("Unaligned [* ,MD] = [* ,MD] not yet implemented");
    }
#ifndef RELEASE
    PopCallStack();
#endif
    return *this;
}

template<typename T,typename Int>
inline const DistMatrix<T,STAR,MD,Int>&
DistMatrix<T,STAR,MD,Int>::operator=( const DistMatrix<T,MR,MC,Int>& A )
{
#ifndef RELEASE
    PushCallStack("[* ,MD] = [MR,MC]");
    this->AssertNotLockedView();
    this->AssertSameGrid( A );
    if( this->Viewing() )
        this->AssertSameSize( A );
#endif
    throw std::logic_error("[* ,MD] = [MR,MC] not yet implemented");
#ifndef RELEASE
    PopCallStack();
#endif
    return *this;
}

template<typename T,typename Int>
inline const DistMatrix<T,STAR,MD,Int>&
DistMatrix<T,STAR,MD,Int>::operator=( const DistMatrix<T,MR,STAR,Int>& A )
{
#ifndef RELEASE
    PushCallStack("[* ,MD] = [MR,* ]");
    this->AssertNotLockedView();
    this->AssertSameGrid( A );
    if( this->Viewing() )
        this->AssertSameSize( A );
#endif
    throw std::logic_error("[* ,MD] = [MR,* ] not yet implemented");
#ifndef RELEASE
    PopCallStack();
#endif
    return *this;
}

template<typename T,typename Int>
inline const DistMatrix<T,STAR,MD,Int>&
DistMatrix<T,STAR,MD,Int>::operator=( const DistMatrix<T,STAR,MC,Int>& A )
{
#ifndef RELEASE
    PushCallStack("[* ,MD] = [* ,MC]");
    this->AssertNotLockedView();
    this->AssertSameGrid( A );
    if( this->Viewing() )
        this->AssertSameSize( A );
#endif
    throw std::logic_error("[* ,MD] = [* ,MC] not yet implemented");
#ifndef RELEASE
    PopCallStack();
#endif
    return *this;
}

template<typename T,typename Int>
inline const DistMatrix<T,STAR,MD,Int>&
DistMatrix<T,STAR,MD,Int>::operator=( const DistMatrix<T,VC,STAR,Int>& A )
{
#ifndef RELEASE
    PushCallStack("[* ,MD] = [VC,* ]");
    this->AssertNotLockedView();
    this->AssertSameGrid( A );
    if( this->Viewing() )
        this->AssertSameSize( A );
#endif
    throw std::logic_error("[* ,MD] = [VC,* ] not yet implemented");
#ifndef RELEASE
    PopCallStack();
#endif
    return *this;
}

template<typename T,typename Int>
inline const DistMatrix<T,STAR,MD,Int>&
DistMatrix<T,STAR,MD,Int>::operator=( const DistMatrix<T,STAR,VC,Int>& A )
{
#ifndef RELEASE
    PushCallStack("[* ,MD] = [* ,VC]");
    this->AssertNotLockedView();
    this->AssertSameGrid( A );
    if( this->Viewing() )
        this->AssertSameSize( A );
#endif
    throw std::logic_error("[* ,MD] = [* ,VC] not yet implemented");
#ifndef RELEASE
    PopCallStack();
#endif
    return *this;
}

template<typename T,typename Int>
inline const DistMatrix<T,STAR,MD,Int>&
DistMatrix<T,STAR,MD,Int>::operator=( const DistMatrix<T,VR,STAR,Int>& A )
{
#ifndef RELEASE
    PushCallStack("[* ,MD] = [VR,* ]");
    this->AssertNotLockedView();
    this->AssertSameGrid( A );
    if( this->Viewing() )
        this->AssertSameSize( A );
#endif
    throw std::logic_error("[* ,MD] = [VR,* ] not yet implemented");
#ifndef RELEASE
    PopCallStack();
#endif
    return *this;
}

template<typename T,typename Int>
inline const DistMatrix<T,STAR,MD,Int>&
DistMatrix<T,STAR,MD,Int>::operator=( const DistMatrix<T,STAR,VR,Int>& A )
{
#ifndef RELEASE
    PushCallStack("[* ,MD] = [* ,VR]");
    this->AssertNotLockedView();
    this->AssertSameGrid( A );
    if( this->Viewing() )
        this->AssertSameSize( A );
#endif
    throw std::logic_error("[* ,MD] = [* ,VR] not yet implemented");
#ifndef RELEASE
    PopCallStack();
#endif
    return *this;
}

template<typename T,typename Int>
inline const DistMatrix<T,STAR,MD,Int>&
DistMatrix<T,STAR,MD,Int>::operator=( const DistMatrix<T,STAR,STAR,Int>& A )
{
#ifndef RELEASE
    PushCallStack("[* ,MD] = [* ,* ]");
    this->AssertNotLockedView();
    this->AssertSameGrid( A );
    if( this->Viewing() )
        this->AssertSameSize( A );
#endif
    if( !this->Viewing() )
        this->ResizeTo( A.Height(), A.Width() );

    if( this->Participating() )
    {
        const Int lcm = this->Grid().LCM();
        const Int rowShift = this->RowShift();

        const Int height = this->Height();
        const Int localWidth = this->LocalWidth();

        T* thisLocalBuffer = this->LocalBuffer();
        const Int thisLDim = this->LocalLDim();
        const T* ALocalBuffer = A.LockedLocalBuffer();
        const Int ALDim = A.LocalLDim();
#ifdef HAVE_OPENMP
        #pragma omp parallel for
#endif
        for( Int jLocal=0; jLocal<localWidth; ++jLocal )
        {
            const T* ACol = &ALocalBuffer[(rowShift+jLocal*lcm)*ALDim];
            T* thisCol = &thisLocalBuffer[jLocal*thisLDim];
            MemCopy( thisCol, ACol, height );
        }
    }
#ifndef RELEASE
    PopCallStack();
#endif
    return *this;
}

//
// Routines which explicitly work in the complex plane
//

template<typename T,typename Int>
inline typename Base<T>::type
DistMatrix<T,STAR,MD,Int>::GetRealPart( Int i, Int j ) const
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::GetRealPart");
    this->AssertValidEntry( i, j );
#endif
    typedef typename Base<T>::type R;

    // We will determine the owner of entry (i,j) and broadcast from it
    const elem::Grid& g = this->Grid();
    const Int r = g.Height();
    const Int c = g.Width();
    const Int ownerRow = (j + this->rowAlignment_) % r;
    const Int ownerCol = (j + this->rowAlignment_ + this->diagPath_) % c;
    const Int ownerRank = ownerRow + r*ownerCol;

    R u;
    if( g.VCRank() == ownerRank )
    {
        const Int jLocal = (j-this->RowShift()) / g.LCM();
        u = this->GetLocalRealPart( i, jLocal );
    }
    mpi::Broadcast( &u, 1, g.VCToViewingMap(ownerRank), g.ViewingComm() );
#ifndef RELEASE
    PopCallStack();
#endif
    return u;
}

template<typename T,typename Int>
inline typename Base<T>::type
DistMatrix<T,STAR,MD,Int>::GetImagPart( Int i, Int j ) const
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::GetImagPart");
    this->AssertValidEntry( i, j );
#endif
    typedef typename Base<T>::type R;

    // We will determine the owner of entry (i,j) and broadcast from it
    const elem::Grid& g = this->Grid();
    const Int r = g.Height();
    const Int c = g.Width();
    const Int ownerRow = (j + this->rowAlignment_) % r;
    const Int ownerCol = (j + this->rowAlignment_ + this->diagPath_) % c;
    const Int ownerRank = ownerRow + r*ownerCol;

    R u;
    if( g.VCRank() == ownerRank )
    {
        const Int jLocal = (j-this->RowShift()) / g.LCM();
        u = this->GetLocalImagPart( i, jLocal );
    }
    mpi::Broadcast( &u, 1, g.VCToViewingMap(ownerRank), g.ViewingComm() );
#ifndef RELEASE
    PopCallStack();
#endif
    return u;
}

template<typename T,typename Int>
inline void
DistMatrix<T,STAR,MD,Int>::SetRealPart
( Int i, Int j, typename Base<T>::type u )
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::SetRealPart");
    this->AssertValidEntry( i, j );
#endif
    const elem::Grid& g = this->Grid();
    const Int r = g.Height();
    const Int c = g.Width();
    const Int ownerRow = (j + this->rowAlignment_) % r;
    const Int ownerCol = (j + this->rowAlignment_ + this->diagPath_) % c;
    const Int ownerRank = ownerRow + r*ownerCol;

    if( g.VCRank() == ownerRank )
    {
        const Int jLocal = (j-this->RowShift()) / g.LCM();
        this->SetLocalRealPart( i, jLocal, u );
    }
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T,typename Int>
inline void
DistMatrix<T,STAR,MD,Int>::SetImagPart
( Int i, Int j, typename Base<T>::type u )
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::SetImagPart");
    this->AssertValidEntry( i, j );
#endif
    if( !IsComplex<T>::val )
        throw std::logic_error("Called complex-only routine with real data");

    const elem::Grid& g = this->Grid();
    const Int r = g.Height();
    const Int c = g.Width();
    const Int ownerRow = (j + this->rowAlignment_) % r;
    const Int ownerCol = (j + this->rowAlignment_ + this->diagPath_) % c;
    const Int ownerRank = ownerRow + r*ownerCol;

    if( g.VCRank() == ownerRank )
    {
        const Int jLocal = (j-this->RowShift()) / g.LCM();
        this->SetLocalImagPart( i, jLocal, u );
    }
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T,typename Int>
inline void
DistMatrix<T,STAR,MD,Int>::UpdateRealPart
( Int i, Int j, typename Base<T>::type u )
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::UpdateRealPart");
    this->AssertValidEntry( i, j );
#endif
    const elem::Grid& g = this->Grid();
    const Int r = g.Height();
    const Int c = g.Width();
    const Int ownerRow = (j + this->rowAlignment_) % r;
    const Int ownerCol = (j + this->rowAlignment_ + this->diagPath_) % c;
    const Int ownerRank = ownerRow + r*ownerCol;

    if( g.VCRank() == ownerRank )
    {
        const Int jLocal = (j-this->RowShift()) / g.LCM();
        this->UpdateLocalRealPart( i, jLocal, u );
    }
#ifndef RELEASE
    PopCallStack();
#endif
}

template<typename T,typename Int>
inline void
DistMatrix<T,STAR,MD,Int>::UpdateImagPart
( Int i, Int j, typename Base<T>::type u )
{
#ifndef RELEASE
    PushCallStack("[* ,MD]::UpdateImagPart");
    this->AssertValidEntry( i, j );
#endif
    if( !IsComplex<T>::val )
        throw std::logic_error("Called complex-only routine with real data");

    const elem::Grid& g = this->Grid();
    const Int r = g.Height();
    const Int c = g.Width();
    const Int ownerRow = (j + this->rowAlignment_) % r;
    const Int ownerCol = (j + this->rowAlignment_ + this->diagPath_) % c;
    const Int ownerRank = ownerRow + r*ownerCol;

    if( g.VCRank() == ownerRank )
    {
        const Int jLocal = (j-this->RowShift()) / g.LCM();
        this->UpdateLocalImagPart( i, jLocal, u );
    }
#ifndef RELEASE
    PopCallStack();
#endif
}

} // namespace elem
