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

export function activate(context: vscode.ExtensionContext) {
  const state: State = { toggled: false };
  const config = vscode.workspace.getConfiguration("ant");

  const disposable = vscode.commands.registerCommand("ant.toggle", () => {
    state.toggled = !state.toggled;

    const editor = vscode.window.activeTextEditor;
    if (editor) {
      if (!state.toggled) {
        editor.setDecorations(annotationText, []);
        return;
      }

      editor.setDecorations(
        annotationText,
        getAnnotations(config.get("binaryPath")).map((annotation) => {
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
        }),
      );
    }
  });

  context.subscriptions.push(disposable);
}

export function deactivate() {}

const getAnnotations = (binaryPath?: string): Annotation[] => {
  const antDir = vscode.workspace.workspaceFolders?.at(0)?.uri.fsPath;
  const file = vscode.window.activeTextEditor?.document.fileName;
  if (!antDir) {
    vscode.window.showInformationMessage("Missing root folder");
    return [];
  }
  if (!file) {
    vscode.window.showInformationMessage("No active editor");
    return [];
  }

  const ant = spawnSync(binaryPath ?? "ant", [
    "-o",
    antDir,
    "list",
    file,
    "--json",
  ]);
  if (ant.error) {
    console.error(ant.error);
  }
  if (ant.status != 0) {
    vscode.window.showInformationMessage(ant.stdout.toString());
    return [];
  }

  return JSON.parse(ant.stdout.toString());
};
