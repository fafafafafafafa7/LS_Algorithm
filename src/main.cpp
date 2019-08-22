#include "BLS.h"
#include "FL.h"
#include "Graph.h"
#include "GraphIO.h"
#include "MoveBlockF.h"
#include "MoveBlockG.h"
#include "Partition.h"
#include "Solver.h"
#include "args.hxx"
#include "utils.h"

std::string inputFile, outputFile;
std::string metisPath;

int timeLim;

Solver *solver;
BLS *bls;
Graph *G;

int algOpt = 1, lsOpt = 0;
int partNum = 30;
int seed = 772002;

double alpha = 1.1;

GraphIO io;

bool inputParam(int argc, char *argv[]) {
    args::ArgumentParser parser(
        "GrEdPa: Implementations of two local search algorithms , LS-G and "
        "LS-F, on Balanced Edge Partition Problem\n");

    args::HelpFlag help(parser, "help", "Display this help menu",
                        {'h', "help"});
    args::Group required(parser, "", args::Group::Validators::All);

    args::ValueFlag<std::string> benchmark_file(
        parser, "benchmark", "Path to benchmark", {'f', "file"},
        "benchmark/socfb-American75.txt");

    args::ValueFlag<std::string> output_file(
        parser, "output file", "Path to output file", {'o', "ouput"},
        "output/output.txt");

    args::ValueFlag<int> Parts(parser, "part number",
                               "The number of parts you want to partition into",
                               {'p', "part"}, 30);

    args::ValueFlag<int> Seed(parser, "seed", "Seed due to the randomness",
                              {'s', "seed"}, 772002);

    args::ValueFlag<double> Alpha(parser, "alpha", "The balance factor",
                                  {'a', "alpha"}, 1.1);

    args::ValueFlag<std::string> Ls(
        parser, "Local search method",
        "The local search method, supported options: "
        "MoveBlockG and MoveBlockF",
        {'l', "local"}, "MoveBlockG");

    args::ValueFlag<std::string> MetisPath(
        parser, "Metis Path", "The Path to metis solver", {'m', "metis"},
        "third_party/metis/gpmetis");

    args::ValueFlag<int> TimeLim(parser, "Time limitation",
                                 "The cut down time in second", {'t', "time"},
                                 90);

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help) {
        std::cout << parser;
        return 0;
    } catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 0;
    } catch (args::ValidationError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 0;
    }

    inputFile = args::get(benchmark_file);
    outputFile = args::get(output_file);

    seed = args::get(Seed);
    partNum = args::get(Parts);

    std::string opt;

    timeLim = args::get(TimeLim);
    metisPath = args::get(MetisPath);

    alpha = args::get(Alpha);

    algOpt = 8;

    opt = args::get(Ls);

    if (opt == "MoveBlockG")
        lsOpt = 1;
    else if (opt == "MoveBlockF")
        lsOpt = 4;
    return true;
}

std::string getPartitionFileName() {
    std::string res = "";
    std::string graphName;
    int p = 0;
    for (int i = 0; i < inputFile.length(); ++ i)
        if (inputFile[i] == '/') p = i;
    graphName.assign(inputFile.begin() + p + 1, inputFile.begin() + inputFile.length() - 4);
    res = "temp_partition/" + graphName + "_" + std::to_string(algOpt) + "_" + std::to_string(seed) + "_" + std::to_string(alpha) + "_" + std::to_string(partNum) + ".txt";
    return res;
}

int main(int argc, char *argv[]) {
    if (!inputParam(argc, argv))
        return 0;

    io.input(G, inputFile);
    srand(seed);

    std::string dirCmd = "if [ -d \"temp\" ]; then rm -d -r temp; mkdir temp; else mkdir temp; fi";
    system(dirCmd.c_str());

    // printf("%d\n", G -> N);

    int lowSize = (double)(G->M) / partNum / alpha + 0.5;
    int upSize = (double)(G->M) / partNum * alpha + 0.5;

    std::string partitionFileName = getPartitionFileName();
    std::ifstream tmpFin(partitionFileName.c_str());
    if (tmpFin && algOpt != 2 && algOpt != 7) {
        algOpt = 8;
    }
    tmpFin.close();

    std::cout << algOpt << std::endl;

    switch (algOpt) {
    

    default:
        solver = new FL(G, partNum, lowSize, upSize);
        solver->setPartitionFile(partitionFileName);
        break;
    }
    // for (int i = 1; i <= 10; i++) printf("%d\n", G -> head[i]);

    solver->solve();

    tmpFin = std::ifstream(partitionFileName.c_str());
    if (!tmpFin && algOpt != 2 && algOpt != 7) {
        tmpFin.close();
        solver->storePartition(partitionFileName.c_str());
    } else {
        tmpFin.close();
    }

    upSize = (double)(G->M) / solver->Pa->PartNum * alpha + 0.5;

    io.output(solver->Pa, outputFile, "Before Local Search", "temp/beforeCost.txt");

    for (int i = 1; i <= solver->Pa->PartNum; ++i) {
        // lowSize = std::min(lowSize, (int)solver->Pa->EP[i].size() - 1);
        upSize = std::max(upSize, (int)solver->Pa->EP[i].size() + 1);
    }

    // if (algOpt == 7) lowSize = 0;

    printf("%d %d\n", lowSize, upSize);

    switch (lsOpt) {
    case 1:
        bls = new MoveBlockG(G, solver->Pa, lowSize, upSize);
        break;

    case 4:
        bls = new MoveBlockF(G, solver->Pa, lowSize, upSize);
        break;

    case 5:
        bls = new MoveBlockG(G, solver->Pa, lowSize, upSize);
        bls->localSearch();
        io.output(solver->Pa, outputFile, "After Rand Local Search", "temp/afterCost.txt");
        bls = new MoveBlockF(G, solver->Pa, lowSize, upSize);
        bls->localSearch();
        break;

    default:
        break;
    }

    if (lsOpt != 0)
        bls->localSearch();

    io.output(solver->Pa, outputFile, "After Local Search",
              "temp/afterCost.txt");

    // printf("haha\n");

    delete G;
    delete solver;
    return 0;
}
