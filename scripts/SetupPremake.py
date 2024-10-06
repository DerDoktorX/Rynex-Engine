import sys
import os
from pathlib import Path

import Utils

class PremakeConfiguration:
    premakeVersion = "5.0.0-beta1"
    premakeZipUrls = f"https://github.com/premake/premake-core/releases/download/v{premakeVersion}/premake-{premakeVersion}-windows.zip"
    premakeLicenseUrl = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
    premakeDirectory = "./vendor/bin/premake"
    premakeDirectoryCustomization = "./vendor/bin/premake/premake_customization"

    @classmethod
    def Validate(cls):
        if (not cls.CheckIfPremakeInstalled()):
            print("Premake is not installed.")
            return False

        print(f"Correct Premake located at {os.path.abspath(cls.premakeDirectory)}")
        return True

    @classmethod
    def CheckIfPremakeInstalled(cls):
        premakeExe = Path(f"{cls.premakeDirectory}/premake5.exe");
        if (not premakeExe.exists()):
            return cls.InstallPremake()

        return True

    @classmethod
    def InstallPremake(cls):
        permissionGranted = False
        while not permissionGranted:
            reply = str(input("Premake not found. Would you like to download Premake {0:s}? [Y/N]: ".format(cls.premakeVersion))).lower().strip()[:1]
            if reply == 'n':
                return False
            permissionGranted = (reply == 'y')

        premakePath = f"{cls.premakeDirectory}/premake-{cls.premakeVersion}-windows.zip"
        print("Downloading {0:s} to {1:s}".format(cls.premakeZipUrls, premakePath))
        Utils.DownloadFile(cls.premakeZipUrls, premakePath)
        print("Extracting", premakePath)
        Utils.UnzipFile(premakePath, deleteZipFile=True)
        print(f"Premake {cls.premakeVersion} has been downloaded to '{cls.premakeDirectory}'")

        


        premakeLicensePath = f"{cls.premakeDirectory}/LICENSE.txt"
        print("Downloading {0:s} to {1:s}".format(cls.premakeLicenseUrl, premakeLicensePath))
        Utils.DownloadFile(cls.premakeLicenseUrl, premakeLicensePath)
        print(f"Premake License file has been downloaded to '{cls.premakeDirectory}'")

        print("Create Customization")
        premakeSolutionItems = f"{cls.premakeDirectory}/solution_items.lua"
        premakeSolutionItems = os.path.abspath(premakeSolutionItems)
        os.makedirs(os.path.dirname(premakeSolutionItems), exist_ok=True)
        with open(premakeSolutionItems, 'w') as f:
            f.write("-- Implement the solution_items command for solution-scope files\nrequire('vstudio')\n\npremake.api.register {\n\tname = 'solution_items',\n\tscope = 'workspace',\n\tkind = 'list:string',\n}\n\npremake.override(premake.vstudio.sln2005, 'projects', function(base, wks)\n\tif wks.solution_items and #wks.solution_items > 0 then\n\t\tlocal solution_folder_GUID = '{2150E333-8FDC-42A3-9474-1A3956D46DE8}' -- See https://www.codeproject.com/Reference/720512/List-of-Visual-Studio-Project-Type-GUIDs\n\t\tpremake.push('Project(\\'' .. solution_folder_GUID .. '\\') = \\'Solution Items\\', \\'Solution Items\\', \\'{' .. os.uuid('Solution Items:' .. wks.name) .. '}\\'')\n\t\tpremake.push('ProjectSection(SolutionItems) = preProject')\n\n\t\tfor _, path in ipairs(wks.solution_items) do\n\t\t\tpremake.w(path .. ' = '.. path)\n\t\tend\n\n\t\tpremake.pop('EndProjectSection')\n\t\tpremake.pop('EndProject')\n\tend\n\tbase(wks)\nend)")

        return True
