##########################################################################################
#                                                                                        #
# Copyright (c) 2016-2026                                                                #
# Alexander Bock                                                                         #
#                                                                                        #
# All rights reserved.                                                                   #
#                                                                                        #
# Redistribution and use in source and binary forms, with or without modification, are   #
# permitted provided that the following conditions are met:                              #
#                                                                                        #
# 1. Redistributions of source code must retain the above copyright notice, this list    #
#    of conditions and the following disclaimer.                                         #
#                                                                                        #
# 2. Redistributions in binary form must reproduce the above copyright notice, this      #
#    list of conditions and the following disclaimer in the documentation and/or other   #
#    materials provided with the distribution.                                           #
#                                                                                        #
# 3. Neither the name of the copyright holder nor the names of its contributors may be   #
#    used to endorse or promote products derived from this software without specific     #
#    prior written permission.                                                           #
#                                                                                        #
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY    #
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES   #
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT    #
# SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,         #
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   #
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR     #
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN       #
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN     #
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH    #
# DAMAGE.                                                                                #
#                                                                                        #
##########################################################################################

<#
.SYNOPSIS
    Regenerates the THIRD_PARTY_LICENSE file from the dependencies installed by vcpkg.

.DESCRIPTION
    Collects the name, version, license, and homepage of every port that vcpkg installed
    into `vcpkg_installed` and concatenates the `copyright` file that each port ships. The
    result is written to THIRD_PARTY_LICENSE in the repository root.

    Run this script after the dependencies in vcpkg.json or the vcpkg baseline change.

.EXAMPLE
    pwsh tools/generate-third-party-license.ps1
#>

[CmdletBinding()]
param (
    [string] $VcpkgInstalled,
    [string] $Triplet,
    [string] $OutputFile
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repositoryRoot = Split-Path -Parent $PSScriptRoot

if (-not $VcpkgInstalled) {
    $VcpkgInstalled = Join-Path $repositoryRoot 'vcpkg_installed'
}
if (-not $OutputFile) {
    $OutputFile = Join-Path $repositoryRoot 'THIRD_PARTY_LICENSE'
}

if (-not (Test-Path -Path $VcpkgInstalled -PathType Container)) {
    throw "Could not find '$VcpkgInstalled'. Configure the project once so that vcpkg installs the dependencies"
}

if (-not $Triplet) {
    $triplets = @(Get-ChildItem -Path $VcpkgInstalled -Directory | Where-Object { $_.Name -ne 'vcpkg' })
    if ($triplets.Count -ne 1) {
        throw "Found $($triplets.Count) triplets in '$VcpkgInstalled'. Specify the one to use with -Triplet"
    }
    $Triplet = $triplets[0].Name
}

$share = Join-Path (Join-Path $VcpkgInstalled $Triplet) 'share'
if (-not (Test-Path -Path $share -PathType Container)) {
    throw "Could not find '$share'"
}

# Ports that only contribute build system helpers and whose code does not end up in any of
# the artifacts that are distributed
$excludedPorts = @(
    'vcpkg-cmake',
    'vcpkg-cmake-config',
    'vcpkg-cmake-get-vars',
    'vcpkg-tool-meson'
)

$packages = @()
foreach ($spdx in Get-ChildItem -Path $share -Recurse -Filter 'vcpkg.spdx.json' -File) {
    $package = (Get-Content -Path $spdx.FullName -Raw | ConvertFrom-Json).packages[0]
    if ($excludedPorts -contains $package.name) {
        continue
    }

    $license = if ($package.PSObject.Properties.Name -contains 'licenseConcluded') {
        $package.licenseConcluded
    }
    else {
        'NOASSERTION'
    }
    $homepage = if ($package.PSObject.Properties.Name -contains 'homepage') {
        $package.homepage
    }
    else {
        ''
    }

    $copyright = Join-Path $spdx.DirectoryName 'copyright'
    $packages += [PSCustomObject]@{
        Name = $package.name
        Version = $package.versionInfo
        License = if ($license -in @('NOASSERTION', 'LicenseRef-vcpkg-null')) {
            'see the notice below'
        }
        else {
            $license
        }
        Homepage = $homepage
        Copyright = if (Test-Path -Path $copyright -PathType Leaf) {
            (Get-Content -Path $copyright -Raw).TrimEnd()
        }
        else {
            $null
        }
    }
}

if ($packages.Count -eq 0) {
    throw "Did not find any packages in '$share'"
}
$packages = @($packages | Sort-Object Name)

$separator = '=' * 90

$output = [System.Text.StringBuilder]::new()
[void]$output.AppendLine($separator)
[void]$output.AppendLine('THIRD PARTY LICENSES')
[void]$output.AppendLine($separator)
[void]$output.AppendLine()
[void]$output.AppendLine('C-Troll, the Editor, the Starter, and the Tray are distributed together with the')
[void]$output.AppendLine('third party libraries listed below. Every library remains under its own license,')
[void]$output.AppendLine('the full text of which is reproduced in this file. The license of C-Troll itself')
[void]$output.AppendLine('is provided in the accompanying LICENSE file.')
[void]$output.AppendLine()
[void]$output.AppendLine("This file was generated from the vcpkg triplet '$Triplet' by running")
[void]$output.AppendLine('`tools/generate-third-party-license.ps1` and has to be regenerated whenever the')
[void]$output.AppendLine('dependencies change.')
[void]$output.AppendLine()
[void]$output.AppendLine()
[void]$output.AppendLine('OVERVIEW')
[void]$output.AppendLine('-' * 90)
[void]$output.AppendLine()

$nameWidth = ($packages | ForEach-Object { $_.Name.Length } | Measure-Object -Maximum).Maximum
$versionWidth = ($packages | ForEach-Object { $_.Version.Length } | Measure-Object -Maximum).Maximum
foreach ($package in $packages) {
    $name = $package.Name.PadRight($nameWidth)
    $version = $package.Version.PadRight($versionWidth)
    [void]$output.AppendLine("  $name  $version  $($package.License)")
    if ($package.Homepage) {
        [void]$output.AppendLine("  $(' ' * $nameWidth)  $(' ' * $versionWidth)  $($package.Homepage)")
    }
}

foreach ($package in $packages) {
    [void]$output.AppendLine()
    [void]$output.AppendLine()
    [void]$output.AppendLine($separator)
    [void]$output.AppendLine("$($package.Name) $($package.Version)")
    if ($package.Homepage) {
        [void]$output.AppendLine($package.Homepage)
    }
    [void]$output.AppendLine($separator)
    [void]$output.AppendLine()
    if ($package.Copyright) {
        [void]$output.AppendLine($package.Copyright)
    }
    else {
        [void]$output.AppendLine('This package does not ship a copyright notice.')
    }
}

$encoding = [System.Text.UTF8Encoding]::new($false)
[System.IO.File]::WriteAllText($OutputFile, $output.ToString(), $encoding)

Write-Host "Wrote $($packages.Count) packages to '$OutputFile'"
