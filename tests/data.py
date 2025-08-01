from dataclasses import dataclass

@dataclass
class Sample:
	input_string: str
	start:        int
	end:          int
	step:         int
	output:       str

random_strings = ['hi my name is pink and im really glad to meet you', 
                  'hey woahhhhh is this illegal',
                  'this feels illegalllllll',
                  'C is a procedural high-level systems programming language created by Ken Thompson and Dennis Ritchie. It was made as the successor to B and intended to be used for writing the UNIX operating system. It was created in 1972 and is the basis for most Linux tools and applications.'
                  ]
starts = [0, 1, 2, 5]
ends   = [1, 3, 5, 12]
steps  = [1, 2, 3, 4]

samples = []
for string in random_strings:
    for start in starts:
        for end in ends:
            for step in steps:
                sample = Sample(string, start, end, step, "")
                sample.output = string[start : end : step]
                samples.append(sample)

csv = open("data.csv", "w")
csv.write("Input,Output,Start,End,Step\n")
print("Input, Output, Start, End, Step")
for sample in samples:
    print("{}, {}, {}, {}, {}".format(sample.input_string, sample.output, sample.start, sample.end, sample.step))
    csv.write("{},{},{},{},{}\n".format(sample.input_string, sample.output, sample.start, sample.end, sample.step))

csv.close()
