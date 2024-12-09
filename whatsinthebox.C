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
#include "argList.H"
#include "fvMesh.H"
#include "indexedOctree.H"
#include "treeDataCell.H"
#include "volFields.H"

using namespace Foam;

int main(int argc, char *argv[])
{
    #include "setRootCase.H"
    
    // Define the bounding box
    scalar minX = 0.025;
    scalar minY = 0.025;
    scalar minZ = 0.0;
    scalar maxX = 0.075;
    scalar maxY = 0.075;
    scalar maxZ = 0.01;

    treeBoundBox searchBox(point(minX, minY, minZ), point(maxX, maxY, maxZ));
    
    // Create time object
    Time runTime(Time::controlDictName, args);
    
    // Load mesh
    fvMesh mesh
    (
        IOobject
        (
            fvMesh::defaultRegion,
            runTime.constant(),
            runTime,
            IOobject::MUST_READ
        )
    );

    // Create visualization field
    volScalarField selectedCells
    (
        IOobject
        (
            "selectedCells",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedScalar("zero", dimless, 0.0)  // Initialize all cells to 0
    );

    // Access the octree from the mesh
    const indexedOctree<treeDataCell>& cellTree = mesh.cellTree();

    // Query the cells in the bounding box
    labelHashSet foundCells;
    label count = cellTree.findBox(searchBox, foundCells);

    // Mark selected cells with value 1
    forAllConstIter(labelHashSet, foundCells, iter)
    {
        selectedCells[*iter] = 1.0;
    }

    // Print results
    Info << "Bounding box: [" << minX << ", " << minY << ", " << minZ
         << "] to [" << maxX << ", " << maxY << ", " << maxZ << "]" << nl;
    Info << "Number of cells found in bounding box: " << count << nl;
    Info << "Cells found:" << nl;
    forAllConstIter(labelHashSet, foundCells, iter)
    {
        Info << *iter << nl;
    }

    // Write the field
    Info<< "Writing selectedCells field to " << runTime.timeName() << endl;
    selectedCells.write();

    return 0;
}

// ************************************************************************* //
