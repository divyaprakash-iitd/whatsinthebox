/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2024 AUTHOR,AFFILIATION
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    whatsinthebox

Description

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#include "argList.H"
#include "fvMesh.H"
#include "indexedOctree.H"
#include "treeDataCell.H"

using namespace Foam;

int main(int argc, char *argv[])
{
    // Parse command-line arguments (for case directory)
    argList args(argc, argv);

    // Define the bounding box directly in the code
    scalar minX = 0.1;
    scalar minY = 0.1;
    scalar minZ = 0.1;
    scalar maxX = 0.3;
    scalar maxY = 0.3;
    scalar maxZ = 0.3;

    treeBoundBox searchBox(point(minX, minY, minZ), point(maxX, maxY, maxZ));

    // Load the mesh from the case directory
    Foam::Time runTime(Foam::Time::controlDictName, args.rootPath(), args.caseName());
    fvMesh mesh(runTime);

    // Access the octree from the mesh using cellTree()
    const indexedOctree<treeDataCell>& cellTree = mesh.cellTree();

    // Query the cells in the bounding box
    labelHashSet foundCells;
    label count = cellTree.findBox(searchBox, foundCells);

    // Print the results
    Info << "Bounding box: [" << minX << ", " << minY << ", " << minZ
         << "] to [" << maxX << ", " << maxY << ", " << maxZ << "]" << nl;

    Info << "Number of cells found in bounding box: " << count << nl;
    Info << "Cells found:" << nl;

    forAllConstIter(labelHashSet, foundCells, iter)
    {
        Info << *iter << nl;
    }

    return 0;
}

// ************************************************************************* //
