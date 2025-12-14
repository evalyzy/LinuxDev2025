table = {
    's': ["ach", "ack", "ad", "age", "ald", "ale", "an", "ang", "ar", "ard",
          "as", "ash", "at", "ath", "augh", "aw", "ban", "bel", "bur", "cer",
          "cha", "che", "dan", "dar", "del", "den", "dra", "dyn", "ech", "eld",
          "elm", "em", "en", "end", "eng", "enth", "er", "ess", "est", "et",
          "gar", "gha", "hat", "hin", "hon", "ia", "ight", "ild", "im", "ina",
          "ine", "ing", "ir", "is", "iss", "it", "kal", "kel", "kim", "kin",
          "ler", "lor", "lye", "mor", "mos", "nal", "ny", "nys", "old", "om",
          "on", "or", "orm", "os", "ough", "per", "pol", "qua", "que", "rad",
          "rak", "ran", "ray", "ril", "ris", "rod", "roth", "ryn", "sam",
          "say", "ser", "shy", "skel", "sul", "tai", "tan", "tas", "ther",
          "tia", "tin", "ton", "tor", "tur", "um", "und", "unt", "urn", "usk",
          "ust", "ver", "ves", "vor", "war", "wor", "yer"],
    'v': ["a", "e", "i", "o", "u", "y"],
    'V': ["a", "e", "i", "o", "u", "y", "ae", "ai", "au", "ay", "ea", "ee",
          "ei", "eu", "ey", "ia", "ie", "oe", "oi", "oo", "ou", "ui"],
    'c': ["b", "c", "d", "f", "g", "h", "j", "k", "l", "m", "n", "p", "q", "r",
          "s", "t", "v", "w", "x", "y", "z"],
    'B': ["b", "bl", "br", "c", "ch", "chr", "cl", "cr", "d", "dr", "f", "g",
          "h", "j", "k", "l", "ll", "m", "n", "p", "ph", "qu", "r", "rh", "s",
          "sch", "sh", "sl", "sm", "sn", "st", "str", "sw", "t", "th", "thr",
          "tr", "v", "w", "wh", "y", "z", "zh"],
    'C': ["b", "c", "ch", "ck", "d", "f", "g", "gh", "h", "k", "l", "ld", "ll",
          "lt", "m", "n", "nd", "nn", "nt", "p", "ph", "q", "r", "rd", "rr",
          "rt", "s", "sh", "ss", "st", "t", "th", "v", "w", "y", "z"],
    'i': ["elch", "idiot", "ob", "og", "ok", "olph", "olt", "omph", "ong",
          "onk", "oo", "oob", "oof", "oog", "ook", "ooz", "org", "ork", "orm",
          "oron", "ub", "uck", "ug", "ulf", "ult", "um", "umb", "ump", "umph",
          "un", "unb", "ung", "unk", "unph", "unt", "uzz"],
    'm': ["baby", "booble", "bunker", "cuddle", "cuddly", "cutie", "doodle",
          "foofie", "gooble", "honey", "kissie", "lover", "lovey", "moofie",
          "mooglie", "moopie", "moopsie", "nookum", "poochie", "poof",
          "poofie", "pookie", "schmoopie", "schnoogle", "schnookie",
          "schnookum", "smooch", "smoochie", "smoosh", "snoogle", "snoogy",
          "snookie", "snookum", "snuggy", "sweetie", "woogle", "woogy",
          "wookie", "wookum", "wuddle", "wuddly", "wuggy", "wunny"],
    'M': ["boo", "bunch", "bunny", "cake", "cakes", "cute", "darling",
          "dumpling", "dumplings", "face", "foof", "goo", "head", "kin",
          "kins", "lips", "love", "mush", "pie", "poo", "pooh", "pook",
          "pums"],
    'D': ["b", "bl", "br", "cl", "d", "f", "fl", "fr", "g", "gh", "gl", "gr",
          "h", "j", "k", "kl", "m", "n", "p", "th", "w"],
    'd': ["air", "ankle", "ball", "beef", "bone", "bum", "bumble", "bump",
          "cheese", "clod", "clot", "clown", "corn", "dip", "dolt", "doof",
          "dork", "dumb", "face", "finger", "foot", "fumble", "goof",
          "grumble", "head", "knock", "knocker", "knuckle", "loaf", "lump",
          "lunk", "meat", "muck", "munch", "nit", "numb", "pin", "puff",
          "skull", "snark", "sneeze", "thimble", "twerp", "twit", "wad",
          "wimp", "wipe"]
}

order = "svVcBCimMDd"

argz = []
flat_offsets = []
type_info = []

for c in order:
    items = table[c]
    current_type_start_idx = len(flat_offsets)
    current_type_count = len(items)
    type_info.append((current_type_start_idx, current_type_count))
    
    for item in items:
        flat_offsets.append(len(argz))
        argz.extend(map(ord, item))
        argz.append(0)

print("static const signed char special[] = {")
for i in range(128):
    if i % 8 == 0: print("    ", end="")
    if chr(i) in order:
        print(f"0x{order.index(chr(i)):02x}, ", end="")
    else:
        print("  -1, ", end="")
    if i % 8 == 7: print()
print("};")

print("static const short offsets_table[] = {")
for i, x in enumerate(flat_offsets):
    if i % 8 == 0: print("    ", end="")
    print(f"0x{x:04x}, ", end="")
    if i % 8 == 7: print()
print("\n};")

print("static const short off_len[] = {")
for start, count in type_info:
    print(f"    0x{start:04x}, 0x{count:04x}, ")
print("};")

print("static const char namegen_argz[] = {")
for i, x in enumerate(argz):
    if i % 15 == 0: print("    ", end="")
    print(f"'{chr(x) if x else 0}', " if x else " 0 , ", end="")
    if i % 15 == 14: print()
print("\n};")
