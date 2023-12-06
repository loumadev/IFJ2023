const path = require("path");

(async function() {
	const now = new Date();

	console.log("Building a project...");

	console.log("Building a PDF...");
	await generatePDF();

	console.log(`Done! (Project built in ${Math.round(new Date() - now)}ms.)`);
})();


async function generatePDF() {
	const puppeteer = require("puppeteer");

	console.log("Launching browser...");
	const browser = await puppeteer.launch({headless: "new"});

	console.log("Opening new page...");
	const page = await browser.newPage();

	console.log("Navigating to target path...");
	await page.goto("https://vut-fit.loumadev.eu/IFJ/projects/01/docs/doc.md?theme=light&__c=" + Math.random(), {
		waitUntil: "networkidle2"
	});

	console.log("Generating pdf...");
	// const height = await page.evaluate(() => document.documentElement.offsetHeight);
	await page.pdf({
		path: path.join(__dirname, "out.pdf"),
		format: "A4",
		// height: `${height}px`,
		printBackground: true,
		displayHeaderFooter: true,
		headerTemplate: `<div style="
			width: 100%;
			display: flex;
			justify-content: space-between;
			align-items: center;
			padding: 10px 20px;
			margin: 0 40px;
			font-size: 12px;
			font-weight: 600;
			border-bottom: 1px solid black;
			opacity: 0.75;
			/*background: pink;*/
			box-sizing: border-box;
			-webkit-print-color-adjust: exact;
		">
			<span>Dokumentácia</span>
			<span>IFJ23</span>
		</div>`,
		footerTemplate: `<div style="
			width: 100%;
			display: flex;
			justify-content: center;
			align-items: center;
			padding: 10px;
			margin: 0 30px;
			font-size: 12px;
			font-weight: 600;
			opacity: 0.75;
			box-sizing: border-box;
			-webkit-print-color-adjust: exact;
		">
			<div>
				<span class="pageNumber"></span> / <span class="totalPages" style="font-size: 10px"></span>
			</div>
		</div>`,
		margin: {
			top: "80px",
			bottom: "70px",
			right: "10px",
			left: "10px",
		},
	});

	console.log("Closing browser...");
	await browser.close();
}