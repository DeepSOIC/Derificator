/***************************************************************************
 *   Copyright (c) Konstantinos Poulios      (logari81@gmail.com) 2011     *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#ifndef FREEGCS_CONSTRAINTS_H
#define FREEGCS_CONSTRAINTS_H

#include "Geo.h"
#include "Util.h"

namespace GCS
{

    ///////////////////////////////////////
    // Constraints
    ///////////////////////////////////////

    enum ConstraintType {
        None = 0,
        Equal = 1,
        Difference = 2,
        P2PDistance = 3,
        P2PAngle = 4,
        P2LDistance = 5,
        PointOnLine = 6,
        PointOnPerpBisector = 7,
        Parallel = 8,
        Perpendicular = 9,
        L2LAngle = 10,
        MidpointOnLine = 11,
        TangentCircumf = 12,
        P2OnEllipse = 13,
        TangentEllipseLine = 14,
        Point2EllipseDistance = 15,
        InternalAlignmentPoint2Ellipse = 16,
        EqualMajorAxesEllipse = 17,
        EllipticalArcRangeToEndPoints = 18
    };
    
    enum InternalAlignmentType {
        EllipsePositiveMajorX = 0,
        EllipsePositiveMajorY = 1,
        EllipseNegativeMajorX = 2,
        EllipseNegativeMajorY = 3,
        EllipsePositiveMinorX = 4,
        EllipsePositiveMinorY = 5,
        EllipseNegativeMinorX = 6,
        EllipseNegativeMinorY = 7,
        EllipseFocus2X = 8,
        EllipseFocus2Y = 9
    };

    class Constraint
    {
    protected:
        VEC_pD origpvec; // is used only as a reference for redirecting and reverting pvec
        VEC_pD pvec;
        double scale;
        int tag;
    public:
        Constraint();
        virtual ~Constraint(){}

        inline VEC_pD params() { return pvec; }

        void redirectParams(MAP_pD_pD redirectionmap);
        void revertParams();
        void setTag(int tagId) { tag = tagId; }
        int getTag() { return tag; }

        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
        // virtual void grad(MAP_pD_D &deriv);  --> TODO: vectorized grad version
        virtual double maxStep(MAP_pD_D &dir, double lim=1.);

		double DoSelfTest(int &iworst_param);
    };

    // Equal
    class ConstraintEqual : public Constraint
    {
    private:
        inline double* param1() { return pvec[0]; }
        inline double* param2() { return pvec[1]; }
    public:
        ConstraintEqual(double *p1, double *p2);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
    };

    // Difference
    class ConstraintDifference : public Constraint
    {
    private:
        inline double* param1() { return pvec[0]; }
        inline double* param2() { return pvec[1]; }
        inline double* difference() { return pvec[2]; }
    public:
        ConstraintDifference(double *p1, double *p2, double *d);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
    };

    // P2PDistance
    class ConstraintP2PDistance : public Constraint
    {
    private:
        inline double* p1x() { return pvec[0]; }
        inline double* p1y() { return pvec[1]; }
        inline double* p2x() { return pvec[2]; }
        inline double* p2y() { return pvec[3]; }
        inline double* distance() { return pvec[4]; }
    public:
        ConstraintP2PDistance(Point &p1, Point &p2, double *d);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
        virtual double maxStep(MAP_pD_D &dir, double lim=1.);
    };

    // P2PAngle
    class ConstraintP2PAngle : public Constraint
    {
    private:
        inline double* p1x() { return pvec[0]; }
        inline double* p1y() { return pvec[1]; }
        inline double* p2x() { return pvec[2]; }
        inline double* p2y() { return pvec[3]; }
        inline double* angle() { return pvec[4]; }
        double da;
    public:
        ConstraintP2PAngle(Point &p1, Point &p2, double *a, double da_=0.);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
        virtual double maxStep(MAP_pD_D &dir, double lim=1.);
    };

    // P2LDistance
    class ConstraintP2LDistance : public Constraint
    {
    private:
        inline double* p0x() { return pvec[0]; }
        inline double* p0y() { return pvec[1]; }
        inline double* p1x() { return pvec[2]; }
        inline double* p1y() { return pvec[3]; }
        inline double* p2x() { return pvec[4]; }
        inline double* p2y() { return pvec[5]; }
        inline double* distance() { return pvec[6]; }
    public:
        ConstraintP2LDistance(Point &p, Line &l, double *d);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
        virtual double maxStep(MAP_pD_D &dir, double lim=1.);
        double abs(double darea);
    };

    // PointOnLine
    class ConstraintPointOnLine : public Constraint
    {
    private:
        inline double* p0x() { return pvec[0]; }
        inline double* p0y() { return pvec[1]; }
        inline double* p1x() { return pvec[2]; }
        inline double* p1y() { return pvec[3]; }
        inline double* p2x() { return pvec[4]; }
        inline double* p2y() { return pvec[5]; }
    public:
        ConstraintPointOnLine(Point &p, Line &l);
        ConstraintPointOnLine(Point &p, Point &lp1, Point &lp2);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
    };

    // PointOnPerpBisector
    class ConstraintPointOnPerpBisector : public Constraint
    {
    private:
        inline double* p0x() { return pvec[0]; }
        inline double* p0y() { return pvec[1]; }
        inline double* p1x() { return pvec[2]; }
        inline double* p1y() { return pvec[3]; }
        inline double* p2x() { return pvec[4]; }
        inline double* p2y() { return pvec[5]; }
    public:
        ConstraintPointOnPerpBisector(Point &p, Line &l);
        ConstraintPointOnPerpBisector(Point &p, Point &lp1, Point &lp2);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
    };

    // Parallel
    class ConstraintParallel : public Constraint
    {
    private:
        inline double* l1p1x() { return pvec[0]; }
        inline double* l1p1y() { return pvec[1]; }
        inline double* l1p2x() { return pvec[2]; }
        inline double* l1p2y() { return pvec[3]; }
        inline double* l2p1x() { return pvec[4]; }
        inline double* l2p1y() { return pvec[5]; }
        inline double* l2p2x() { return pvec[6]; }
        inline double* l2p2y() { return pvec[7]; }
    public:
        ConstraintParallel(Line &l1, Line &l2);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
    };

    // Perpendicular
    class ConstraintPerpendicular : public Constraint
    {
    private:
        inline double* l1p1x() { return pvec[0]; }
        inline double* l1p1y() { return pvec[1]; }
        inline double* l1p2x() { return pvec[2]; }
        inline double* l1p2y() { return pvec[3]; }
        inline double* l2p1x() { return pvec[4]; }
        inline double* l2p1y() { return pvec[5]; }
        inline double* l2p2x() { return pvec[6]; }
        inline double* l2p2y() { return pvec[7]; }
    public:
        ConstraintPerpendicular(Line &l1, Line &l2);
        ConstraintPerpendicular(Point &l1p1, Point &l1p2, Point &l2p1, Point &l2p2);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
    };

    // L2LAngle
    class ConstraintL2LAngle : public Constraint
    {
    private:
        inline double* l1p1x() { return pvec[0]; }
        inline double* l1p1y() { return pvec[1]; }
        inline double* l1p2x() { return pvec[2]; }
        inline double* l1p2y() { return pvec[3]; }
        inline double* l2p1x() { return pvec[4]; }
        inline double* l2p1y() { return pvec[5]; }
        inline double* l2p2x() { return pvec[6]; }
        inline double* l2p2y() { return pvec[7]; }
        inline double* angle() { return pvec[8]; }
    public:
        ConstraintL2LAngle(Line &l1, Line &l2, double *a);
        ConstraintL2LAngle(Point &l1p1, Point &l1p2,
                           Point &l2p1, Point &l2p2, double *a);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
        virtual double maxStep(MAP_pD_D &dir, double lim=1.);
    };

    // MidpointOnLine
    class ConstraintMidpointOnLine : public Constraint
    {
    private:
        inline double* l1p1x() { return pvec[0]; }
        inline double* l1p1y() { return pvec[1]; }
        inline double* l1p2x() { return pvec[2]; }
        inline double* l1p2y() { return pvec[3]; }
        inline double* l2p1x() { return pvec[4]; }
        inline double* l2p1y() { return pvec[5]; }
        inline double* l2p2x() { return pvec[6]; }
        inline double* l2p2y() { return pvec[7]; }
    public:
        ConstraintMidpointOnLine(Line &l1, Line &l2);
        ConstraintMidpointOnLine(Point &l1p1, Point &l1p2, Point &l2p1, Point &l2p2);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
    };

    // TangentCircumf
    class ConstraintTangentCircumf : public Constraint
    {
    private:
        inline double* c1x() { return pvec[0]; }
        inline double* c1y() { return pvec[1]; }
        inline double* c2x() { return pvec[2]; }
        inline double* c2y() { return pvec[3]; }
        inline double* r1() { return pvec[4]; }
        inline double* r2() { return pvec[5]; }
        bool internal;
    public:
        ConstraintTangentCircumf(Point &p1, Point &p2,
                                 double *rd1, double *rd2, bool internal_=false);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
    };
    // PointOnEllipse
    class ConstraintPointOnEllipse : public Constraint
    {
    private:
        inline double* p1x() { return pvec[0]; }
        inline double* p1y() { return pvec[1]; }
        inline double* cx() { return pvec[2]; }
        inline double* cy() { return pvec[3]; }
        inline double* f1x() { return pvec[4]; }
        inline double* f1y() { return pvec[5]; }
        inline double* rmin() { return pvec[6]; }
    public:
        ConstraintPointOnEllipse(Point &p, Ellipse &e);
        ConstraintPointOnEllipse(Point &p, ArcOfEllipse &a);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
    };
    
    class ConstraintEllipseTangentLine : public Constraint
    {
    private:
        inline double* p1x() { return pvec[0]; }
        inline double* p1y() { return pvec[1]; }
        inline double* p2x() { return pvec[2]; }
        inline double* p2y() { return pvec[3]; }        
        inline double* cx() { return pvec[4]; }
        inline double* cy() { return pvec[5]; }
        inline double* f1x() { return pvec[6]; }
        inline double* f1y() { return pvec[7]; }
        inline double* rmin() { return pvec[8]; }
    public:
        ConstraintEllipseTangentLine(Line &l, Ellipse &e);
        ConstraintEllipseTangentLine(Line &l, ArcOfEllipse &a);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
    };
        
    class ConstraintInternalAlignmentPoint2Ellipse : public Constraint
    {
    private:
        inline double* p1x() { return pvec[0]; }
        inline double* p1y() { return pvec[1]; }      
        inline double* cx() { return pvec[2]; }
        inline double* cy() { return pvec[3]; }
        inline double* f1x() { return pvec[4]; }
        inline double* f1y() { return pvec[5]; }
        inline double* rmin() { return pvec[6]; }
    public:
        ConstraintInternalAlignmentPoint2Ellipse(Ellipse &e, Point &p1, InternalAlignmentType alignmentType);
        ConstraintInternalAlignmentPoint2Ellipse(ArcOfEllipse &e, Point &p1, InternalAlignmentType alignmentType);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
    private:
        InternalAlignmentType AlignmentType;
    };
    
    class ConstraintEqualMajorAxesEllipse : public Constraint
    {
    private:     
        inline double* e1cx() { return pvec[0]; }
        inline double* e1cy() { return pvec[1]; }
        inline double* e1f1x() { return pvec[2]; }
        inline double* e1f1y() { return pvec[3]; }
        inline double* e1rmin() { return pvec[4]; }
        inline double* e2cx() { return pvec[5]; }
        inline double* e2cy() { return pvec[6]; }
        inline double* e2f1x() { return pvec[7]; }
        inline double* e2f1y() { return pvec[8]; }
        inline double* e2rmin() { return pvec[9]; }
    public:
        ConstraintEqualMajorAxesEllipse(Ellipse &e1, Ellipse &e2);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
    };
    
    // EllipticalArcRangeToEndPoints
    class ConstraintEllipticalArcRangeToEndPoints : public Constraint
    {
    private:
        inline double* p1x() { return pvec[0]; }
        inline double* p1y() { return pvec[1]; }
        inline double* angle() { return pvec[2]; }
        inline double* cx() { return pvec[3]; }
        inline double* cy() { return pvec[4]; }
        inline double* f1x() { return pvec[5]; }
        inline double* f1y() { return pvec[6]; }
        inline double* rmin() { return pvec[7]; }
    public:
        ConstraintEllipticalArcRangeToEndPoints(Point &p, ArcOfEllipse &a, double *angle_t);
        virtual ConstraintType getTypeId();
        virtual void rescale(double coef=1.);
        virtual double error();
        virtual double grad(double *);
        virtual double maxStep(MAP_pD_D &dir, double lim=1.);
    };
    
    //template<typename typEllipse>
    double point2EllipseTheta(double px, double py, ArcOfEllipse e);

} //namespace GCS

#endif // FREEGCS_CONSTRAINTS_H
