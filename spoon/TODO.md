Todo:
-----

- Add function declarations (forward declaration etc.)#
- Add type checking
- Create "object" class containing the file's definitions
	- make it (de)serializable?
- Create linker program
- have NFC etc. hardcoded and get some simple code generation going!
- add labels (loopStart:)
	- type of statement
	- also have local scope - local link @ compile time
	- during link, we push the addresses of labels we pass
	- then look them up... may require multi-pass! (unless labels must be above jump)
	- or keep a map of machine addresses to symbols that should have been subbed in, and retroactively sub the label vectors in after passing through the tree.
- more control structures (goto, if, while - these should all translate into NFCs.)
- bulk out the standard library
- add comments to tokenizer!