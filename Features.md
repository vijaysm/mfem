# Detailed description of MFEM's features

# MFEM Features

The goal of MFEM is to enable research and development of scalable finite element discretization and solver algorithms through general finite element abstractions, accurate and flexible visualization, and tight integration with the *[hypre](http://www.llnl.gov/CASC/hypre)* library.

Conceptually, MFEM can be viewed as a finite element toolbox that provides the building blocks for developing finite element algorithms in a manner similar to that of MATLAB for linear algebra methods.

## Higher-order Finite Element Spaces

<img src="http://images.mfem.googlecode.com/hg/ho-spaces-padding.png" align="right" alt="H(curl) and H(div) spaces">

MFEM supports a wide variety of [finite element](http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_16.svg) [spaces](http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_17.svg) in 2D and 3D, including arbitrary high-order:
 - [H1](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?h1)-conforming, [H(div)](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?hdiv)-conforming, [H(curl)](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?hcurl)-conforming spaces,
 - discontinuous [L2](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?l2) spaces,
 - numerical trace ([interfacial](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?h12)) spaces,
 - [NURBS](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?nurbs) spaces for isogeometric analysis.

Many [bilinear](http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_67.svg) and [linear](http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_40.svg) forms defined on these spaces, as well as linear operators such as gradient, curl and embedding between these spaces, are available in the code.

## Flexible Discretization

In addition to classical Galerkin methods, MFEM enables the quick prototyping of
 - [mixed](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?mixed) finite elements,
 - Discontinuous Galerkin ([DG](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?dg)) methods,
 - [isogeometric](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?nurbs) analysis methods,
 - Discontinuous Petrov-Galerkin ([DPG](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?dpg)) approaches.

<wiki:comment>
http://doxygen.mfem.googlecode.com/hg/examples/README_files/ex9.png
</wiki:comment>

<img src="http://doxygen.mfem.googlecode.com/hg/examples/README_files/ex6.png" align="right" width="230">

## Wide Range of Mesh Types

MFEM supports arbitrary element [transformations](http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_14.svg) and includes classes for dealing with:
 - triangular, quadrilateral, tetrahedral and hexahedral [elements](http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_13.svg),
 - conforming local mesh refinement (triangular/tetrahedral meshes),
 - non-conforming mesh refinement (quadrilateral/hexahedral meshes), including anisotropic refinement,
 - higher-order elements with [curved](http://code.google.com/p/glvis/wiki/CurvilinearVTKMeshes) boundaries,
 - [surface](https://code.google.com/p/mfem/source/browse/data/square-disc-surf.mesh) meshes embedded in 3D,
 - topologically [periodic](https://code.google.com/p/mfem/source/browse/data/periodic-hexagon.mesh) meshes,
 - 1D meshes.

## Parallel and Scalable

MFEM supports MPI-based parallelism throughout the library, and can readily be used as a scalable unstructured finite element problem generator.
 - MFEM-based [applications](http://www.llnl.gov/casc/blast) have been [scaled](http://www.llnl.gov/casc/blast/parallel.php) to [hundreds of thousands](http://computation.llnl.gov/project/blast/images/blast_strong_vulcan_inline.png) of parallel cores.
 - An experimental support for OpenMP acceleration is also included.

<wiki:comment>
<img src="http://computation.llnl.gov/project/blast/images/blast_strong_vulcan_inline.png", width=400>
</wiki:comment>

A serial MFEM application typically requires [minimal](http://doxygen.mfem.googlecode.com/hg/html/ex1_8cpp_source.html) [changes](http://doxygen.mfem.googlecode.com/hg/html/ex1p_8cpp_source.html) to transition to a high-performing parallel version of the code, where it can take advantage of the integrated scalable linear solvers from the *[hypre](http://www.llnl.gov/CASC/hypre)* library.

## Built-in Solvers

MFEM is commonly used as a "finite element to linear algebra translator", since it can take a problem described in terms of finite element-type objects, and produce the corresponding linear algebra
[vectors](http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_102.svg) and [sparse matrices](http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_72.svg).

Several matrix storage formats are available including dense, compressed sparse row ([CSR](http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1SparseMatrix.html)) and parallel compressed sparse row ([ParCSR](http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1HypreParMatrix.html)). Block vectors, operators and [matrices](http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1BlockMatrix.html) are also supported.

A variety of solvers are available for the resulting linear algebra systems (or semi-discrete time-integration problems):
 - point-wise and polynomial [serial](http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1SparseSmoother.html) and [parallel](http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1HypreSmoother.html) smoothers,
<img src="https://computation.llnl.gov/project/linear_solvers/images/hypre_wiw.gif" align="right" width="250">
 - [Krylov solvers](http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1IterativeSolver.html), such as PCG, MINRES and GMRES applicable to general [operators](http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_72.svg) in serial and in parallel,
 - high-performance preconditioners from the *[hypre](http://www.llnl.gov/CASC/hypre)* library including the [BoomerAMG](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?amg), [AMS](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?ams) and [ADS](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html?ads) solvers,
 - sequential sparse direct solvers from the [SuiteSparse](http://www.cise.ufl.edu/research/sparse/SuiteSparse) library,
 - explicit and implicit high-order Runge-Kutta [time integrators](http://doxygen.mfem.googlecode.com/hg/html/inherit_graph_71.svg),
 - solvers for nonlinear problems (Newton) and for single linearly constrained [quadratic minimization](http://doxygen.mfem.googlecode.com/hg/html/classmfem_1_1SLBQPOptimizer.html) problems.

## Extensive Examples

MFEM includes a number of well-documented [example codes](http://doxygen.mfem.googlecode.com/hg/examples/README_files/index.html) that can be used as tutorials, as well as simple starting points for user applications. Some of the included example codes are:

 - [Example 1](http://doxygen.mfem.googlecode.com/hg/html/ex1_8cpp_source.html): nodal H1 FEM for the Laplace problem,
 - [Example 2](http://doxygen.mfem.googlecode.com/hg/html/ex2_8cpp_source.html): vector FEM for linear elasticity,
 - [Example 3](http://doxygen.mfem.googlecode.com/hg/html/ex3_8cpp_source.html): Nedelec H(curl) FEM for the definite Maxwell problem,
 - [Example 4](http://doxygen.mfem.googlecode.com/hg/html/ex4_8cpp_source.html): Raviart-Thomas H(div) FEM for the grad-div problem,
 - [Example 5](http://doxygen.mfem.googlecode.com/hg/html/ex5_8cpp_source.html): mixed pressure-velocity FEM for the Darcy problem,
 - [Example 6](http://doxygen.mfem.googlecode.com/hg/html/ex6_8cpp_source.html): non-conforming adaptive mesh refinement for the Laplace problem,
 - [Example 7](http://doxygen.mfem.googlecode.com/hg/html/ex7_8cpp_source.html): Laplace problem on a surface (the unit sphere),
 - [Example 8](http://doxygen.mfem.googlecode.com/hg/html/ex8_8cpp_source.html): Discontinuous Petrov-Galerkin (DPG) for the Laplace problem,
 - [Example 9](http://doxygen.mfem.googlecode.com/hg/html/ex9_8cpp_source.html): Discontinuous Galerkin (DG) time-dependent advection,
 - [Example 10](http://doxygen.mfem.googlecode.com/hg/html/ex10_8cpp_source.html): time-dependent implicit nonlinear elasticity.

Most of the examples have a serial and a parallel version, illustrating the ease of transition and the minimal code changes between the two.

<wiki:comment>
<img src="http://images.glvis.googlecode.com/hg/logo-130.png" align="right" alt="GLVis logo">
</wiki:comment>

## Accurate and Flexible Visualization

The general (high-order) meshes and finite element functions in MFEM can be visualized accurately using the companion OpenGL visualization tool [GLVis](http://glvis.googlecode.com), which is built on top of MFEM.

<wiki:comment>
http://glvis.googlecode.com/files/glvis-screenshot2.png
</wiki:comment>

The [VisIt](http://visit.llnl.gov) visualization and analysis tool also natively supports MFEM formats.

## Lightweight, Portable and Easily Extendable

The MFEM code base is [[Releases|relatively small]] and is written in highly portable C++ (e.g. with very limited use of templates and the STL).

 - The serial version of MFEM has no external dependencies and is [[Building|straightforward to build]] on Linux, Mac and Windows machines.
 - The MPI-parallel version uses two third-party libraries (*hypre* and METIS), and is also easy to build with an MPI compiler.
 - On most machines, both versions can be built in under a minute by typing: "`make serial -j`" and "`make parallel -j`" respectively.

The object-oriented design of MFEM [separates](http://doxygen.mfem.googlecode.com/hg/html/index.html) the mesh, finite element and linear algebra abstractions, making it easy to extend the library and adapt it to a variety of [[Publications|applications]].

## Open Source

MFEM is an open-source software, and can be freely used under the terms of the [LGPL 2.1](https://www.gnu.org/licenses/lgpl-2.1.html) license.