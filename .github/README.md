# GitHub Templates and Workflows

This directory contains GitHub-specific configuration files for the WarpTPS repository.

## Files

### Issue Templates

Located in `.github/ISSUE_TEMPLATE/`:

- **`bug_report.md`** - Template for reporting bugs
  - Use when: You've found a problem or unexpected behavior
  - Includes: Environment details, reproduction steps, expected vs actual behavior

- **`feature_request.md`** - Template for requesting new features
  - Use when: You have an idea for a new capability
  - Includes: Problem statement, proposed solution, use cases, API design

### Pull Request Template

- **`pull_request_template.md`** - Automatically loaded when creating a PR
  - Comprehensive checklist for code quality
  - Build verification steps
  - Testing requirements
  - Documentation updates
  - Breaking changes documentation

## Usage

### Creating an Issue

1. Go to the [Issues tab](https://github.com/dg1an3/WarpTPS/issues)
2. Click "New Issue"
3. Select the appropriate template:
 - ?? Bug Report
   - ? Feature Request
4. Fill out all sections of the template
5. Submit the issue

### Creating a Pull Request

1. Push your changes to a feature branch
2. Go to the [Pull Requests tab](https://github.com/dg1an3/WarpTPS/pulls)
3. Click "New Pull Request"
4. Select your branch
5. The PR template will auto-load - fill it out completely
6. Submit for review

### Tips for Good Issues/PRs

**Issues:**
- Be specific and detailed
- Include code examples or screenshots
- Provide system/environment information
- Search for similar issues first

**Pull Requests:**
- Reference related issues
- Keep changes focused and atomic
- Include tests for new functionality
- Update documentation
- Respond to review feedback promptly

## Customization

These templates can be modified to fit project needs:

1. Edit the template files in this directory
2. Commit and push changes
3. Templates will update immediately on GitHub

## Related Files

- `CONTRIBUTING.md` - Detailed contribution guidelines (repository root)
- `CLAUDE.md` - Architectural and development guidance (repository root)
- `.gitmessage` - Commit message template (repository root)
- `setup_git.bat/ps1` - Git configuration scripts (repository root)

## Workflow Integration

These templates integrate with:

- **GitHub Actions** - CI/CD workflows validate PR requirements
- **Branch Protection** - Enforce review and testing requirements
- **Issue Tracking** - Organize work and track progress
- **Project Boards** - Visualize workflow (if configured)

## Questions?

See `CONTRIBUTING.md` for detailed guidelines or ask in discussions.
