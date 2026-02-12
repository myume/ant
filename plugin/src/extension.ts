import { spawnSync } from "child_process";
import * as vscode from "vscode";

type Annotation = {
  annotation: string;
  source: string;
  row: number;
};

type State = {
  toggled: boolean;
};

const state: State = { toggled: false };

const annotationText = vscode.window.createTextEditorDecorationType({
  after: {
    color: new vscode.ThemeColor("editorGhostText.foreground"),
    margin: "0 0 0 1em",
  },
});

function getTruncatedText(text: string, maxLength: number): string {
  if (text.length <= maxLength) return text;
  return text.substring(0, maxLength - 3) + "...";
}

const getAnnotations = (
  file: string,
  binaryPath: string,
  sourceRoot: string,
): Annotation[] => {
  const ant = spawnSync(binaryPath ?? "ant", [
    "-s",
    sourceRoot,
    "-o",
    sourceRoot,
    "list",
    file.replace(sourceRoot + "/", ""),
    "--json",
  ]);
  if (ant.error) {
    console.error(ant.error);
    return [];
  }
  if (ant.status != 0) {
    vscode.window.showInformationMessage(ant.stdout.toString());
    return [];
  }

  return JSON.parse(ant.stdout.toString());
};

const refreshAnnotations = (
  editor: vscode.TextEditor,
  binaryPath: string,
  sourceRoot: string,
) => {
  if (!state.toggled) {
    editor.setDecorations(annotationText, []);
    return;
  }

  editor.setDecorations(
    annotationText,
    getAnnotations(editor.document.uri.fsPath, binaryPath, sourceRoot).map(
      (annotation) => {
        const line = editor.document.lineAt(annotation.row - 1);
        const range = new vscode.Range(line.range.end, line.range.end);
        return {
          range,
          hoverMessage: new vscode.MarkdownString(annotation.annotation),
          renderOptions: {
            after: {
              contentText: getTruncatedText(annotation.annotation, 80),
            },
          },
        };
      },
    ),
  );
};

// HOVER
class AntHoverProvider implements vscode.HoverProvider {
  private binaryPath: string;
  private sourceRoot: string;

  constructor(binaryPath: string, sourceRoot: string) {
    this.binaryPath = binaryPath;
    this.sourceRoot = sourceRoot;
  }

  public provideHover(
    document: vscode.TextDocument,
    position: vscode.Position,
  ): vscode.Hover | Thenable<vscode.Hover> {
    if (!state.toggled) {
      return new vscode.Hover([]);
    }

    const annotations = getAnnotations(
      document.uri.fsPath,
      this.binaryPath,
      this.sourceRoot,
    );
    const annotation = annotations.find((a) => a.row === position.line + 1);

    const contents = new vscode.MarkdownString();
    contents.isTrusted = true;

    if (annotation) {
      const removeArgs = [document.uri, position.line + 1];
      const removeUri = vscode.Uri.parse(
        `command:ant.remove?${encodeURIComponent(JSON.stringify(removeArgs))}`,
      );
      contents.appendMarkdown(`[remove annotation](${removeUri})`);
    } else {
      const addArgs = [document.uri, position.line + 1];
      const addUri = vscode.Uri.parse(
        `command:ant.add?${encodeURIComponent(JSON.stringify(addArgs))}`,
      );
      contents.appendMarkdown(`[add annotation](${addUri})`);
    }

    return new vscode.Hover(contents);
  }
}

export function activate(context: vscode.ExtensionContext) {
  const config = vscode.workspace.getConfiguration("ant");
  const binaryPath = config.get("binaryPath") as string;
  const sourceRoot = vscode.workspace.workspaceFolders?.at(0)?.uri.fsPath;
  if (!sourceRoot) {
    vscode.window.showInformationMessage("Missing root folder");
    return;
  }

  const hoverProvider = new AntHoverProvider(binaryPath, sourceRoot);

  const toggle = vscode.commands.registerCommand("ant.toggle", () => {
    state.toggled = !state.toggled;
    vscode.window.visibleTextEditors.forEach((editor) =>
      refreshAnnotations(editor, binaryPath, sourceRoot),
    );
  });

  const add = vscode.commands.registerCommand(
    "ant.add",
    async (file: vscode.Uri, row: number) => {
      const annotation = await vscode.window.showInputBox({
        prompt: "Enter your annotation",
      });
      if (!annotation) {
        return;
      }

      console.log([
        "-s",
        sourceRoot,
        "-o",
        sourceRoot,
        "add",
        `${file.fsPath.replace(sourceRoot + "/", "")}:${row}`,
        annotation,
      ]);
      const ant = spawnSync(binaryPath ?? "ant", [
        "-s",
        sourceRoot,
        "-o",
        sourceRoot,
        "add",
        `${file.fsPath.replace(sourceRoot + "/", "")}:${row}`,
        annotation,
      ]);

      if (ant.error) {
        console.error(ant.error);
        return;
      }
      vscode.window.showInformationMessage(ant.stdout.toString());
      if (ant.status != 0) {
        return;
      }

      if (vscode.window.activeTextEditor)
        refreshAnnotations(
          vscode.window.activeTextEditor,
          binaryPath,
          sourceRoot,
        );
    },
  );

  const remove = vscode.commands.registerCommand(
    "ant.remove",
    (file: vscode.Uri, row: number) => {
      const ant = spawnSync(binaryPath ?? "ant", [
        "-s",
        sourceRoot,
        "-o",
        sourceRoot,
        "remove",
        `${file.fsPath.replace(sourceRoot + "/", "")}:${row}`,
      ]);

      if (ant.error) {
        console.error(ant.error);
        return;
      }
      vscode.window.showInformationMessage(ant.stdout.toString());
      if (ant.status != 0) {
        return;
      }

      if (vscode.window.activeTextEditor)
        refreshAnnotations(
          vscode.window.activeTextEditor,
          binaryPath,
          sourceRoot,
        );
    },
  );

  vscode.workspace.onDidChangeConfiguration(() => {
    vscode.commands.executeCommand("ant.toggle");
    vscode.commands.executeCommand("ant.toggle");
  });

  vscode.window.onDidChangeVisibleTextEditors((editors) => {
    editors.forEach((editor) =>
      refreshAnnotations(editor, binaryPath, sourceRoot),
    );
  });

  context.subscriptions.push(toggle, add, remove);
  context.subscriptions.push(
    vscode.languages.registerHoverProvider("*", hoverProvider),
  );
}

export function deactivate() {}
