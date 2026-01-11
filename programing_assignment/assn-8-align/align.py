#!/usr/bin/env python2

import random # for seed, random
import sys    # for stdout
import copy   # for copy

# Computes the score of the optimal alignment of two DNA strands.
def findOptimalAlignment(strand1, strand2, optimalAlignments):
        if (strand1, strand2) in optimalAlignments:
                return copy.copy(optimalAlignments[(strand1, strand2)])
        
        best = {}
        
        # if one of the two strands is empty, then there is only
        # one possible alignment, and of course it's optimal
        if len(strand1) == 0:
                best["strand1"] = " " * len(strand2)
                best["strand2"] = strand2
                best["score"] = len(strand2) * -2
                best["positiveScores"] = " " * len(strand2)
                best["negativeScores"] = "2" * len(strand2)
                optimalAlignments[(strand1, strand2)] = best
                return copy.copy(best)
        if len(strand2) == 0:
                best["strand1"] = strand1
                best["strand2"] = " " * len(strand1)
                best["score"] = len(strand1) * -2
                best["positiveScores"] = " " * len(strand1)
                best["negativeScores"] = "2" * len(strand1)
                optimalAlignments[(strand1, strand2)] = best
                return copy.copy(best)

        # There's the scenario where the two leading bases of
        # each strand are forced to align, regardless of whether or not
        # they actually match.
        bestWith = findOptimalAlignment(strand1[1:], strand2[1:], optimalAlignments)
        best["strand1"] = strand1[0] + bestWith["strand1"]
        best["strand2"] = strand2[0] + bestWith["strand2"]
        if strand1[0] == strand2[0]:
                best["score"] = bestWith["score"] + 1
                best["positiveScores"] = "1" + bestWith["positiveScores"]
                best["negativeScores"] = " " + bestWith["negativeScores"]
                optimalAlignments[(strand1, strand2)] = copy.copy(best)
                return copy.copy(best) # no benefit from making other recursive calls

        best["score"] = bestWith["score"] - 1
        best["positiveScores"] = " " + bestWith["positiveScores"]
        best["negativeScores"] = "1" + bestWith["negativeScores"]
        
        # It's possible that the leading base of strand1 best
        # matches not the leading base of strand2, but the one after it.
        bestWithout = findOptimalAlignment(strand1, strand2[1:], optimalAlignments)
        bestWithout["score"] -= 2 # penalize for insertion of space
        if bestWithout["score"] > best["score"]:
                bestWithout["strand1"] = " " + bestWithout["strand1"]
                bestWithout["strand2"] = strand2[0] + bestWithout["strand2"]
                bestWithout["positiveScores"] = " " + bestWithout["positiveScores"]
                bestWithout["negativeScores"] = "2" + bestWithout["negativeScores"]
                best = bestWithout

        # opposite scenario
        bestWithout = findOptimalAlignment(strand1[1:], strand2, optimalAlignments)
        bestWithout["score"] -= 2 # penalize for insertion of space     
        if bestWithout["score"] > best["score"]:
                bestWithout["strand1"] = strand1[0] + bestWithout["strand1"]
                bestWithout["strand2"] = " " + bestWithout["strand2"]
                bestWithout["positiveScores"] = " " + bestWithout["positiveScores"]
                bestWithout["negativeScores"] = "2" + bestWithout["negativeScores"]
                best = bestWithout

        optimalAlignments[(strand1, strand2)] = best
        return copy.copy(best)

# Utility function that generates a random DNA string of
# a random length drawn from the range [minlength, maxlength]
def generateRandomDNAStrand(minlength, maxlength):
        assert minlength > 0, \
               "Minimum length passed to generateRandomDNAStrand" \
               "must be a positive number" # these \'s allow mult-line statements
        assert maxlength >= minlength, \
               "Maximum length passed to generateRandomDNAStrand must be at " \
               "as large as the specified minimum length"
        strand = ""
        length = random.choice(xrange(minlength, maxlength + 1))
        bases = ['A', 'T', 'G', 'C']
        for i in xrange(0, length):
                strand += random.choice(bases)
        return strand

# Method that just prints out the supplied alignment score.
# This is more of a placeholder for what will ultimately
# print out not only the score but the alignment as well.

def printAlignment(alignment, out = sys.stdout):
        
        out.write("\nOptimal alignment score is " + str(alignment["score"]) + "\n\n")
        out.write("  + " + str(alignment["positiveScores"]) + "\n")
        out.write("    " + str(alignment["strand1"]) + "\n")
        out.write("    " + str(alignment["strand2"]) + "\n")
        out.write("  - " + str(alignment["negativeScores"]) + "\n\n")

# Unit test main in place to do little more than
# exercise the above algorithm.  As written, it
# generates two fairly short DNA strands and
# determines the optimal alignment score.
#
# As you change the implementation of findOptimalAlignment
# to use memoization, you should change the 8s to 40s and
# the 10s to 60s and still see everything execute very
# quickly.
 
def main():
        while (True):
                sys.stdout.write("Generate random DNA strands? ")
                answer = sys.stdin.readline()
                if answer == "no\n": break
                strand1 = generateRandomDNAStrand(40, 60)
                strand2 = generateRandomDNAStrand(40, 60)
                sys.stdout.write("Aligning these two strands:\n\n")
                sys.stdout.write("   " + strand1 + "\n")
                sys.stdout.write("   " + strand2 + "\n")
                optimalAlignments = {}
                alignment = findOptimalAlignment(strand1, strand2, optimalAlignments)
                printAlignment(alignment)
                
if __name__ == "__main__":
  main()
