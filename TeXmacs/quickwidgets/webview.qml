import QtWebView 1.1

WebView {
        id: webView
        url: "https://www.texmacs.org/"
        onLoadingChanged: {
            if (loadRequest.errorString)
                console.error(loadRequest.errorString);
        }
}
