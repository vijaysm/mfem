// Copyright (c) 2010, Lawrence Livermore National Security, LLC. Produced at
// the Lawrence Livermore National Laboratory. LLNL-CODE-443211. All Rights
// reserved. See file COPYRIGHT for details.
//
// This file is part of the MFEM library. For more information and source code
// availability see http://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License (as published by the Free
// Software Foundation) version 2.1 dated February 1999.
//
//            -----------------------------------------------------
//            Mesh Explorer Miniapp:  Explore and manipulate meshes
//            -----------------------------------------------------
//
// This miniapp is a handy tool to examine, visualize and manipulate a given
// mesh. Some of its features are:
//
//    - visualizing of mesh materials and individual mesh elements
//    - mesh scaling, randomization, and general transformation
//    - manipulation of the mesh curvature
//    - the ability to simulate parallel partitioning
//    - quantitative and visual reports of mesh quality
//
// Compile with: make mesh-explorer
//
// Sample runs:  mesh-explorer
//               mesh-explorer -m ../../data/beam-tri.mesh
//               mesh-explorer -m ../../data/star-q2.mesh
//               mesh-explorer -m ../../data/disc-nurbs.mesh
//               mesh-explorer -m ../../data/escher-p3.mesh
//               mesh-explorer -m ../../data/mobius-strip.mesh

#include "mfem.hpp"
#include <fstream>
#include <limits>
#include <cstdlib>

using namespace mfem;
using namespace std;

void transformation(const Vector &p, Vector &v)
{
   // simple shear transformation
   double s = 0.1;
   v(0) = p(0) + s*p(1) + s*p(2);
   v(1) = p(1) + s*p(2) + s*p(0);
   v(2) = p(2);
}

Mesh *read_par_mesh(int np, const char *mesh_prefix)
{
   Mesh *mesh;
   Array<Mesh *> mesh_array;

   mesh_array.SetSize(np);
   ifstream meshin;
   for (int p = 0; p < np; p++)
   {
      ostringstream fname;
      fname << mesh_prefix << '.' << setfill('0') << setw(6) << p;
      meshin.open(fname.str().c_str());
      if (!meshin)
      {
         cerr << "Can not open mesh file: " << fname.str().c_str()
              << '!' << endl;
         for (p--; p >= 0; p--)
         {
            delete mesh_array[p];
         }
         return NULL;
      }
      mesh_array[p] = new Mesh(meshin, 1, 0);
      // set element and boundary attributes to be the processor number + 1
      if (1)
      {
         for (int i = 0; i < mesh_array[p]->GetNE(); i++)
         {
            mesh_array[p]->GetElement(i)->SetAttribute(p+1);
         }
         for (int i = 0; i < mesh_array[p]->GetNBE(); i++)
         {
            mesh_array[p]->GetBdrElement(i)->SetAttribute(p+1);
         }
      }
      meshin.close();
   }
   mesh = new Mesh(mesh_array, np);

   for (int p = 0; p < np; p++)
   {
      delete mesh_array[np-1-p];
   }
   mesh_array.DeleteAll();

   return mesh;
}

int main (int argc, char *argv[])
{
   int np = 0;
   const char *mesh_file = "../../data/beam-hex.mesh";

   OptionsParser args(argc, argv);
   args.AddOption(&mesh_file, "-m", "--mesh",
                  "Mesh file to visualize.");
   args.AddOption(&np, "-np", "--num-proc",
                  "Load mesh from multiple processors.");
   args.Parse();
   if (!args.Good())
   {
      if (!args.Help())
      {
         args.PrintError(cout);
         cout << endl;
      }
      cout << "Visualize and manipulate a serial mesh:\n"
           << "   mesh-explorer -m <mesh_file>\n"
           << "Visualize and manipulate a parallel mesh:\n"
           << "   mesh-explorer -np <#proc> -m <mesh_prefix>\n" << endl
           << "All Options:\n";
      args.PrintHelp(cout);
      return 1;
   }
   args.PrintOptions(cout);

   Mesh *mesh;
   if (np <= 0)
   {
      ifstream imesh(mesh_file);
      if (!imesh)
      {
         cerr << "can not open mesh file: " << mesh_file << endl;
         return 2;
      }
      mesh = new Mesh(imesh, 1, 1);
      imesh.close();
   }
   else
   {
      mesh = read_par_mesh(np, mesh_file);
      if (mesh == NULL)
      {
         return 3;
      }
   }
   int dim  = mesh->Dimension();
   int sdim = mesh->SpaceDimension();

   FiniteElementCollection *attr_fec;
   if (dim == 2)
   {
      attr_fec = new Const2DFECollection;
   }
   else
   {
      attr_fec = new Const3DFECollection;
   }

   int print_char = 1;
   while (1)
   {
      if (print_char)
      {
         cout << endl;
         mesh->PrintCharacteristics();
         cout << "boundary attribs   :";
         for (int i = 0; i < mesh->bdr_attributes.Size(); i++)
         {
            cout << ' ' << mesh->bdr_attributes[i];
         }
         cout << '\n' << "material attribs   :";
         for (int i = 0; i < mesh->attributes.Size(); i++)
         {
            cout << ' ' << mesh->attributes[i];
         }
         cout << endl;
         cout << "mesh curvature     : ";
         if (mesh->GetNodalFESpace() != NULL)
         {
            cout << mesh->GetNodalFESpace()->FEColl()->Name() << endl;
         }
         else
         {
            cout << "NONE" << endl;
         }
      }
      print_char = 0;
      cout << endl;
      cout << "What would you like to do?\n"
           "q) Quit\n"
           "r) Refine\n"
           "c) Change curvature\n"
           "s) Scale\n"
           "t) Transform\n"
           "j) Jitter\n"
           "v) View\n"
           "m) View materials\n"
           "b) View boundary\n"
           "e) View elements\n"
           "h) View element sizes, h\n"
           "k) View element ratios, kappa\n"
           "x) Print sub-element stats\n"
           "p) Generate a partitioning\n"
           "S) Save\n"
           "--> " << flush;
      char mk;
      cin >> mk;

      if (mk == 'q')
      {
         break;
      }

      if (mk == 'r')
      {
         mesh->UniformRefinement();
         print_char = 1;
      }

      if (mk == 'c')
      {
         int p;
         cout << "enter new order for mesh curvature --> " << flush;
         cin >> p;
         mesh->SetCurvature(p);
         print_char = 1;
      }

      if (mk == 's')
      {
         double factor;
         cout << "scaling factor ---> " << flush;
         cin >> factor;

         GridFunction *nodes = mesh->GetNodes();
         if (nodes == NULL)
         {
            for (int i = 0; i < mesh->GetNV(); i++)
            {
               double *v = mesh->GetVertex(i);
               v[0] *= factor;
               v[1] *= factor;
               if (dim == 3)
               {
                  v[2] *= factor;
               }
            }
         }
         else
         {
            *nodes *= factor;
         }

         print_char = 1;
      }

      if (mk == 't')
      {
         mesh->Transform(transformation);
         print_char = 1;
      }

      if (mk == 'j')
      {
         double jitter;
         cout << "jitter factor ---> " << flush;
         cin >> jitter;

         GridFunction *nodes = mesh->GetNodes();

         if (nodes == NULL)
         {
            cerr << "The mesh should have nodes, introduce curvature first!\n";
         }
         else
         {
            FiniteElementSpace *fespace = nodes->FESpace();

            GridFunction rdm(fespace);
            rdm.Randomize();
            rdm -= 0.5; // shift to random values in [-0.5,0.5]
            rdm *= jitter;

            // compute minimal local mesh size
            Vector h0(fespace->GetNDofs());
            h0 = std::numeric_limits<double>::infinity();
            {
               Array<int> dofs;
               for (int i = 0; i < fespace->GetNE(); i++)
               {
                  fespace->GetElementDofs(i, dofs);
                  for (int j = 0; j < dofs.Size(); j++)
                  {
                     h0(dofs[j]) = std::min(h0(dofs[j]), mesh->GetElementSize(i));
                  }
               }
            }

            // scale the random values to be of order of the local mesh size
            for (int i = 0; i < fespace->GetNDofs(); i++)
            {
               for (int d = 0; d < dim; d++)
               {
                  rdm(fespace->DofToVDof(i,d)) *= h0(i);
               }
            }

            int bdr = 0;
            // don't perturb the boundary
            if (!bdr)
            {
               Array<int> vdofs;
               for (int i = 0; i < fespace->GetNBE(); i++)
               {
                  fespace->GetBdrElementVDofs(i, vdofs);
                  for (int j = 0; j < vdofs.Size(); j++)
                  {
                     rdm(vdofs[j]) = 0.0;
                  }
               }
            }

            *nodes += rdm;
         }

         print_char = 1;
      }

      if (mk == 'x')
      {
         int sd, nz = 0;
         DenseMatrix J(dim);
         double min_det_J, max_det_J, min_det_J_z, max_det_J_z;
         double min_kappa, max_kappa, max_ratio_det_J_z;
         min_det_J = min_kappa = numeric_limits<double>::infinity();
         max_det_J = max_kappa = max_ratio_det_J_z = -min_det_J;
         cout << "subdivision factor ---> " << flush;
         cin >> sd;
         for (int i = 0; i < mesh->GetNE(); i++)
         {
            int geom = mesh->GetElementBaseGeometry(i);
            ElementTransformation *T = mesh->GetElementTransformation(i);

            RefinedGeometry *RefG = GlobGeometryRefiner.Refine(geom, sd, 1);
            IntegrationRule &ir = RefG->RefPts;

            min_det_J_z = numeric_limits<double>::infinity();
            max_det_J_z = -min_det_J_z;
            for (int j = 0; j < ir.GetNPoints(); j++)
            {
               T->SetIntPoint(&ir.IntPoint(j));
               Geometries.JacToPerfJac(geom, T->Jacobian(), J);

               double det_J = J.Det();
               double kappa =
                  J.CalcSingularvalue(0) / J.CalcSingularvalue(dim-1);

               min_det_J_z = fmin(min_det_J_z, det_J);
               max_det_J_z = fmax(max_det_J_z, det_J);

               min_kappa = fmin(min_kappa, kappa);
               max_kappa = fmax(max_kappa, kappa);
            }
            max_ratio_det_J_z =
               fmax(max_ratio_det_J_z, max_det_J_z/min_det_J_z);
            min_det_J = fmin(min_det_J, min_det_J_z);
            max_det_J = fmax(max_det_J, max_det_J_z);
            if (min_det_J_z <= 0.0)
            {
               nz++;
            }
         }
         cout
               << "\nbad elements = " << nz
               << "\nmin det(J)   = " << min_det_J
               << "\nmax det(J)   = " << max_det_J
               << "\nglobal ratio = " << max_det_J/min_det_J
               << "\nmax el ratio = " << max_ratio_det_J_z
               << "\nmin kappa    = " << min_kappa
               << "\nmax kappa    = " << max_kappa << endl;
      }

      if (mk == 'm' || mk == 'b' || mk == 'e' || mk == 'v' || mk == 'h' ||
          mk == 'k' || mk == 'p')
      {
         Array<int> part(mesh->GetNE());
         FiniteElementSpace *attr_fespace =
            new FiniteElementSpace(mesh, attr_fec);
         GridFunction attr(attr_fespace);

         if (mk == 'm')
            for (int i = 0; i < mesh->GetNE(); i++)
            {
               part[i] = (attr(i) = mesh->GetAttribute(i)) - 1;
            }

         if (mk == 'b' || mk == 'v')
         {
            attr = 1.0;
         }

         if (mk == 'e')
         {
            Array<int> coloring;
            srandom(time(0));
            double a = double(random()) / (double(RAND_MAX) + 1.);
            int el0 = (int)floor(a * mesh->GetNE());
            cout << "Generating coloring starting with element " << el0+1
                 << " / " << mesh->GetNE() << endl;
            mesh->GetElementColoring(coloring, el0);
            for (int i = 0; i < coloring.Size(); i++)
            {
               attr(i) = coloring[i];
            }
            cout << "Number of colors: " << attr.Max() + 1 << endl;
            for (int i = 0; i < mesh->GetNE(); i++)
            {
               // part[i] = i; // checkerboard element coloring
               attr(i) = part[i] = i; // coloring by element number
            }
         }

         if (mk == 'h')
         {
            DenseMatrix J(dim);
            double h_min, h_max;
            h_min = numeric_limits<double>::infinity();
            h_max = -h_min;
            for (int i = 0; i < mesh->GetNE(); i++)
            {
               int geom = mesh->GetElementBaseGeometry(i);
               ElementTransformation *T = mesh->GetElementTransformation(i);
               T->SetIntPoint(&Geometries.GetCenter(geom));
               Geometries.JacToPerfJac(geom, T->Jacobian(), J);

               attr(i) = J.Det();
               if (attr(i) < 0.0)
               {
                  attr(i) = -pow(-attr(i), 1.0/double(dim));
               }
               else
               {
                  attr(i) = pow(attr(i), 1.0/double(dim));
               }
               h_min = min(h_min, attr(i));
               h_max = max(h_max, attr(i));
            }
            cout << "h_min = " << h_min << ", h_max = " << h_max << endl;
         }

         if (mk == 'k')
         {
            DenseMatrix J(dim);
            for (int i = 0; i < mesh->GetNE(); i++)
            {
               int geom = mesh->GetElementBaseGeometry(i);
               ElementTransformation *T = mesh->GetElementTransformation(i);
               T->SetIntPoint(&Geometries.GetCenter(geom));
               Geometries.JacToPerfJac(geom, T->Jacobian(), J);
               attr(i) = J.CalcSingularvalue(0) / J.CalcSingularvalue(dim-1);
            }
         }

         if (mk == 'p')
         {
            int *partitioning = NULL, n;
            cout << "What type of partitioning?\n"
                 "c) Cartesian\n"
                 "0) METIS_PartGraphRecursive (sorted neighbor lists)\n"
                 "1) METIS_PartGraphKway      (sorted neighbor lists)\n"
                 "2) METIS_PartGraphVKway     (sorted neighbor lists)\n"
                 "3) METIS_PartGraphRecursive\n"
                 "4) METIS_PartGraphKway\n"
                 "5) METIS_PartGraphVKway\n"
                 "--> " << flush;
            char pk;
            cin >> pk;
            if (pk == 'c')
            {
               int nxyz[3];
               cout << "Enter nx: " << flush;
               cin >> nxyz[0]; n = nxyz[0];
               if (mesh->Dimension() > 1)
               {
                  cout << "Enter ny: " << flush;
                  cin >> nxyz[1]; n *= nxyz[1];
                  if (mesh->Dimension() > 2)
                  {
                     cout << "Enter nz: " << flush;
                     cin >> nxyz[2]; n *= nxyz[2];
                  }
               }
               partitioning = mesh->CartesianPartitioning(nxyz);
            }
            else
            {
               int part_method = pk - '0';
               if (part_method < 0 || part_method > 5)
               {
                  continue;
               }
               cout << "Enter number of processors: " << flush;
               cin >> n;
               partitioning = mesh->GeneratePartitioning(n, part_method);
            }
            if (partitioning)
            {
               const char part_file[] = "partitioning.txt";
               ofstream opart(part_file);
               opart << "number_of_elements " << mesh->GetNE() << '\n'
                     << "number_of_processors " << n << '\n';
               for (int i = 0; i < mesh->GetNE(); i++)
               {
                  opart << partitioning[i] << '\n';
               }
               cout << "Partitioning file: " << part_file << endl;

               Array<int> proc_el(n);
               proc_el = 0;
               for (int i = 0; i < mesh->GetNE(); i++)
               {
                  proc_el[partitioning[i]]++;
               }
               int min_el = proc_el[0], max_el = proc_el[0];
               for (int i = 1; i < n; i++)
               {
                  if (min_el > proc_el[i])
                  {
                     min_el = proc_el[i];
                  }
                  if (max_el < proc_el[i])
                  {
                     max_el = proc_el[i];
                  }
               }
               cout << "Partitioning stats:\n"
                    << "           "
                    << setw(12) << "minimum"
                    << setw(12) << "average"
                    << setw(12) << "maximum"
                    << setw(12) << "total" << '\n';
               cout << " elements  "
                    << setw(12) << min_el
                    << setw(12) << double(mesh->GetNE())/n
                    << setw(12) << max_el
                    << setw(12) << mesh->GetNE() << endl;
            }
            else
            {
               continue;
            }

            for (int i = 0; i < mesh->GetNE(); i++)
            {
               attr(i) = part[i] = partitioning[i];
            }
            delete [] partitioning;
         }

         char vishost[] = "localhost";
         int  visport   = 19916;
         socketstream sol_sock(vishost, visport);
         if (sol_sock.is_open())
         {
            sol_sock.precision(14);
            if (sdim == 2)
            {
               sol_sock << "fem2d_gf_data_keys\n";
               if (mk != 'p')
               {
                  mesh->Print(sol_sock);
               }
               else
               {
                  // NURBS meshes do not support PrintWithPartitioning
                  if (mesh->NURBSext)
                  {
                     mesh->Print(sol_sock);
                     for (int i = 0; i < mesh->GetNE(); i++)
                     {
                        attr(i) = part[i];
                     }
                  }
                  else
                  {
                     mesh->PrintWithPartitioning(part, sol_sock, 1);
                  }
               }
               attr.Save(sol_sock);
               sol_sock << "RjlmAb***********";
               if (mk == 'v')
               {
                  sol_sock << "e";
               }
               else
               {
                  sol_sock << "\n";
               }
            }
            else
            {
               sol_sock << "fem3d_gf_data_keys\n";
               if (mk == 'b' || mk == 'v' || mk == 'h' || mk == 'k')
               {
                  mesh->Print(sol_sock);
               }
               else
               {
                  // NURBS meshes do not support PrintWithPartitioning
                  if (mesh->NURBSext)
                  {
                     mesh->Print(sol_sock);
                     for (int i = 0; i < mesh->GetNE(); i++)
                     {
                        attr(i) = part[i];
                     }
                  }
                  else
                  {
                     mesh->PrintWithPartitioning(part, sol_sock);
                  }
               }
               attr.Save(sol_sock);
               sol_sock << "maaA";
               if (mk == 'v')
               {
                  sol_sock << "aa";
               }
               else
               {
                  sol_sock << "\n";
               }
            }
            sol_sock << flush;
         }
         else
            cout << "Unable to connect to "
                 << vishost << ':' << visport << endl;
         delete attr_fespace;
      }

      if (mk == 'S')
      {
         const char mesh_file[] = "mesh-explorer.mesh";
         ofstream omesh(mesh_file);
         omesh.precision(14);
         mesh->Print(omesh);
         cout << "New mesh file: " << mesh_file << endl;
      }
   }

   delete attr_fec;
   delete mesh;
}
