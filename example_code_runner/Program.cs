using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

namespace Cjm.Numerics.Example
{
    class Program
    {
        static void Main(string[] args)
        {
            var workingDirectory = new DirectoryInfo(@"C:\Users\Christopher Susie\Source\Repos\Int128\example_code\src\uint128_example");
            var dict = GetRunInfo(workingDirectory);
            if (!Configurations.SetEquals(dict.Keys))
            {
                Console.WriteLine($"Of the {Configurations.Count} configurations expected, {dict.Count} were found.");
                foreach (var missing in Configurations.Except(dict.Keys))
                {
                    Console.WriteLine($"File for configuration [{missing}] not found.");
                }
            }
            Console.WriteLine("Running the tasks. ... ");
            List<Task<(bool Ok, string ConfigName, string StdOutput, string StdError)>> tasks = (from kvp in dict
                select RunProgramAsync(kvp.Key, kvp.Value)).ToList();
            TimeSpan maxTime = TimeSpan.FromSeconds(10);
            List<(bool Ok, string ConfigName, string StdOutput, string StdError)> taskResults =
                new List<(bool Ok, string ConfigName, string StdOutput, string StdError)>(tasks.Count);
            

            foreach (var task in tasks)
            {
                try
                {
                    task.Wait(maxTime);
                    var tpl = task.Result;
                    taskResults.Add(tpl);
                    Console.WriteLine($"Task for config [{tpl.ConfigName}] completed {(tpl.Ok ? "Successfully" : "FAULT")}.");
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Task for config threw exception: [{ex}].");
                }
                Console.WriteLine();
            }

            Console.WriteLine("Logging task results to output.txt ... ");
            Console.WriteLine();
            var output = new FileInfo("output.txt");
            using var sw = output.CreateText();
            foreach (var res in taskResults)
            {
                sw.WriteLine($"For config {res.ConfigName}: ");
                if (!string.IsNullOrWhiteSpace(res.StdError))
                {
                    sw.WriteLine("STANDARD ERROR: ");
                    sw.WriteLine(res.StdError);
                    sw.WriteLine("END STD ERROR");
                }

                if (!string.IsNullOrWhiteSpace(res.StdOutput))
                {
                    sw.WriteLine("STANDARD OUT: ");
                    sw.WriteLine(res.StdOutput);
                    sw.WriteLine("END STANDARD OUT");
                }
                sw.WriteLine($"DONE for config {res.ConfigName}.");
                sw.WriteLine();
            } 
        }

        

        private static Task<(bool Ok, string ConfigName, string StdOutput, string StdError)> RunProgramAsync(string config, FileInfo fi) => Task.Run( async delegate
        {
            string stdOut = string.Empty;
            string stdErr = string.Empty;
            bool ok;
            try
            {
                using Process p = new Process();
                p.StartInfo.UseShellExecute = false;
                p.StartInfo.FileName = fi.FullName;
                p.StartInfo.RedirectStandardOutput = true;
                p.StartInfo.RedirectStandardError = true;
                p.Start();
                Task<string> stdOutputTask = p.StandardOutput.ReadToEndAsync();
                Task<string> stdErrTask = p.StandardError.ReadToEndAsync();
                await p.WaitForExitAsync();
                stdOut = await stdOutputTask;
                stdErr = await stdErrTask;
                ok = p.ExitCode == 0;
            }
            catch (Exception ex)
            {
                Console.Error.WriteLine(ex);
                ok = false;
            }

            return (ok, config, stdOut, stdErr);
        });

        private static ImmutableSortedDictionary<string, FileInfo> GetRunInfo(
            DirectoryInfo workingDirectory)
        {
            var bldr = ImmutableSortedDictionary.CreateBuilder<string, FileInfo>();
            bldr.AddRange(FindAllFiles(workingDirectory).Select((config) =>
                new KeyValuePair<string, FileInfo>(config.ConfigName, config.ExeFile)));
            return bldr.ToImmutable();
        }
         
        static IEnumerable<(string ConfigName, FileInfo ExeFile)> FindAllFiles(DirectoryInfo di)
        {
            return (from tupl in ConfigRelPathLookup
                let tempTarget = FindFileInfo(di, tupl.RelativePathToExe, ExeFileName)
                where tempTarget != null
                select (tupl.ConfigName, tempTarget!))!;

            static FileInfo? FindFileInfo(DirectoryInfo di, string relPathFolder, string fileName)
            {
                FileInfo? ret = null;
                try
                {
                    ret = new FileInfo(Path.Combine(di.FullName, relPathFolder, fileName));
                }
                catch (Exception ex)
                {
                    Console.Error.WriteLine(ex);
                }

                if (ret?.Exists == false)
                {
                    ret = null;
                }

                return ret;
            }
        }

        static Program()
        {
            ConfigRelPathLookup =  ImmutableArray<(string ConfigName, string RelativePathToExe)>.Empty.AddRange(
            new [] {
                //development branches
                ("Debug-Clang-x86", "Debug-Clang"),
                ("Debug-Intel-x86", "Debug-Llvm-Intel"),
                ("Debug-Microsoft-x86", "Debug-Microsoft"),

                ("Release-Clang-x86", "Release-Clang"),
                ("Release-Intel-x86", "Release-Llvm-Intel"),
                ("Release-Ms-x86", "Release-Microsoft"),

                ("Debug-Clang-x64", "x64/Debug-Clang"),
                ("Debug-Intel-x64", "x64/Debug-Llvm-Intel"),
                ("Debug-Ms-x64", "x64/Debug-Microsoft"),

                ("Release-Clang-x64", "x64/Release-Clang"),
                ("Release-Intel-x64", "x64/Release-Llvm-Intel"),
                ("Release-Ms-x64", "x64/Release-Microsoft"),
                //installed library branches
                ("Inst-Debug-Clang-x86", "Inst-Dbg-Cl"),
                ("Inst-Debug-Intel-x86", "Inst-Dbg-Intl"),
                ("Inst-Debug-Microsoft-x86", "Inst-Dbg-Ms"),

                ("Inst-Release-Clang-x86", "Inst-Rel-Cl"),
                ("Inst-Release-Intel-x86", "Inst-Rel-Intl"),
                ("Inst-Release-Ms-x86", "Inst-Rel-Ms"),

                ("Inst-Debug-Clang-x64", "x64/Inst-Dbg-Cl"),
                ("Inst-Debug-Intel-x64", "x64/Inst-Dbg-Intl"),
                ("Inst-Debug-Ms-x64", "x64/Inst-Dbg-Ms"),

                ("Inst-Release-Clang-x64", "x64/Inst-Rel-Cl"),
                ("Inst-Release-Intel-x64", "x64/Inst-Rel-Intl"),
                ("Inst-Release-Ms-x64", "x64/Inst-Rel-Ms"),

            });
            Configurations = ConfigRelPathLookup.Select(tupl => tupl.ConfigName).ToImmutableSortedSet();
        }

        private static readonly ImmutableArray<(string ConfigName, string RelativePathToExe)> ConfigRelPathLookup;
        private static readonly ImmutableSortedSet<string> Configurations;

        private const string ExeFileName = "uint128_example.exe";
    }
}
