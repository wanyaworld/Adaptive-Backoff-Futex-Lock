git-hooks
==========

The collection of git hooks. Please add hooks whichever you like to
your git hooks under .git/hooks/*

## pre-commit hooks

To apply pre-commit hooks, make .git/hooks/pre-commit file and
add shebang at the beginning of the file.

``` bash
#!/usr/bin/env bash
```

Then, make pre-commit executable.

``` shell
$ chmod +x .git/hooks/pre-commit
```

### Apply pre-commit-hooks

Just add execution command at .git/hooks/pre-commit.
For example, to apply pre-commit-yapf.sh, add the following
at the **pre-commit** file.

``` bash
. ./git-hooks/pre-commit-yapf.sh
```

Note that whereever you run `git commit` command,
git hooks will be executed at the source root.

### type of pre-commit-hooks

* `pre-commit-yapf`

  * Apply `yapf` (python style formatter) to all modified python files.

  * Requirements: [yapf](https://github.com/google/yapf). `pip install yapf` to install.

  * To modify settings, edit .style.yapf.

* `pre-commit-clang-format`

  * Apply `clang-format` (C++ style formatter) to all modified C++ files.

  * Requirements: `clang-format`. Install with conda.

  * To modify settings, edit .clang-format.
