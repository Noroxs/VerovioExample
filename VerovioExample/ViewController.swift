//
//  ViewController.swift
//  VerovioExample
//
//  Created by Thomas Ramson on 10.12.19.
//

import UIKit
import WebKit

class ViewController: UIViewController {

    @IBOutlet weak var webView: WKWebView!

    override func viewDidLoad() {
        super.viewDidLoad()

        let meiURL = Bundle.main.url(forResource: "Chopin_Etude_op.10_no.9", withExtension: "mei")!
        let svg = VerovioWrapper().renderFirstPage(url: meiURL, size: view.bounds.size)

        webView.loadHTMLString(svg, baseURL: nil)
    }

}
