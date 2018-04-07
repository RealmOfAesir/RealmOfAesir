The following is a set of guidelines for contributing to Realm of Aesir.

## What should I know before I get started?

### Copyright Assignment Agreement

Realm of Aesir works with a Copyright Assignment Agreement. If you're making a code contribution for the first time, you will be asked to digitally sign and e-mail an agreement to sign over all your copyright claims of the code to the guardian of Realm of Aesir. Currently that's Michael de Lang but in the future this will most likely be a legal entity. This is necessary for Realm of Aesir to prevent copyright claims and relicense the code for third parties. If you do not sign the agreement, your code contribution will be declined.

You can find the agreement for individuals [here](Realm%20of%20Aesir-Individual.pdf) and the agreement for entities [here](Realm%20of%20Aesir-Entity.pdf). Please fill it in and e-mail it to realm.of.aesir@gmail.com.

### Design Decisions

Most of this project is designed by Michael de Lang, so for any big change please contact him.

## How can I contribute?

### Reporting bugs

This section guides you through submitting a bug report for Realm of Aesir. Following these guidelines helps maintainers and the community understand your report, reproduce the behaviour and find related reports.

#### Before submitting a bug report

* Check the open & closed issues for any related issue. It is not a problem if you can't find one even though there is one, you will get a proper response linking to the already existing issue and your issue will be closed to keep the issue list clean.

#### How do I submit a (good) bug report?

Bugs are tracked as [GitHub issues](https://guides.github.com/features/issues/). After you've determined which repository your bug is related to, create an issue on that repository and provide the following information by filling in [the template](ISSUE_TEMPLATE.md).

Explain the problem and include additional details to help maintainers reproduce the problem:

* **Use a clear and descriptive title** for the issue to identify the problem.
* **Describe the exact steps which reproduce the problem** in as many details as possible. For example, start by explaining how you started Realm of Aesir, e.g. which command exactly you used in the terminal, or how you started it otherwise. When listing steps, **don't just say what you did, but explain how you did it**. For example, if you moved the cursor to button, explain if you used the mouse, or a keyboard shortcut or a command, and if so which one?
* **Provide specific examples to demonstrate the steps**. Include links to files or GitHub projects, or copy/pasteable snippets, which you use in those examples. If you're providing snippets in the issue, use [Markdown code blocks](https://help.github.com/articles/markdown-basics/#multiple-lines).
* **Describe the behavior you observed after following the steps** and point out what exactly is the problem with that behavior.
* **Explain which behavior you expected to see instead and why.**
* **Include screenshots and animated GIFs** which show you following the described steps and clearly demonstrate the problem. You can use [this tool](http://www.cockos.com/licecap/) to record GIFs on macOS and Windows, and [this tool](https://github.com/colinkeenan/silentcast) or [this tool](https://github.com/GNOME/byzanz) on Linux.
* **If you're reporting that Realm of Aesir crashed**, include a crash report with a stack trace from the operating system. The stack trace will be in the logs/ folder of your installation. Include the crash report in the issue in a [code block](https://help.github.com/articles/markdown-basics/#multiple-lines), a [file attachment](https://help.github.com/articles/file-attachments-on-issues-and-pull-requests/), or put it in a [gist](https://gist.github.com/) and provide link to that gist.
* **If the problem wasn't triggered by a specific action**, describe what you were doing before the problem happened and share more information using the guidelines below.

Provide more context by answering these questions:

* **Did the problem start happening recently** (e.g. after updating to a new version of Realm of Aesir) or was this always a problem?
* If the problem started happening recently, **can you reproduce the problem in an older version of Realm of Aesir?** What's the most recent version in which the problem doesn't happen? You can download older versions of Realm of Aesir from [the releases page](https://github.com/RealmOfAesir/client/releases).
* **Can you reliably reproduce the issue?** If not, provide details about how often the problem happens and under which conditions it normally happens.

Include details about your configuration and environment:

* **Which version of Realm of Aesir are you using?** You can get the exact version by running `RealmOfAesir -v` in your terminal, or by starting Realm of Aesir and opening the `Application: About` menu.
* **What's the name and version of the OS you're using**?
* **Which keyboard layout are you using?** Are you using a US layout or some other layout?

## Styleguides

### Git Commit Messages

* Use the present tense ("Add feature" not "Added feature")
* Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
* Limit the first line to 72 characters or less
* Reference issues and pull requests liberally
