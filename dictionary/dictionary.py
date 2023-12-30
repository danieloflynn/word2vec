
from nltk.corpus import wordnet as wn
print("Done downloading")

dictionary = open("nouns.txt", "w")

i = 0
for synset in list(wn.all_synsets('n')):
    if ("-" in synset.lemmas()[0].name() or "_" in synset.lemmas()[0].name()):
        continue
    dictionary.write(synset.lemmas()[0].name().lower() + "\n")
    i += 1
    if i % 10000 == 0:
        print(f"Wrote line {i}")


dictionary.close()
print("finished.")
