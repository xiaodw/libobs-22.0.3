<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:template match="Controls">
		<html>
			<head>
				<title>Duilib属性表</title>
				<style>
					body
					{
						background-color:rgb(255,255,255);
					}
					.title
					{
						font-size:22px;
						font-family:宋体;
						font-weight:bold;
						align: center;
					}
					.default
					{
						font-size:16px;
						font-family:宋体;
					}
					.label-1
					{
						font-size:20px;
						font-family:宋体;
						font-weight:bold;
						height:50px;
						line-height:50px;
					}
					.label-2
					{
						font-size:18px;
						font-family:黑体;
						height:30px;
						line-height:30px;
					}
					.style-table-0
					{
						border-collapse: collapse;
						border-width: 0px;
					}
					.style-table-1
					{
						border-collapse: collapse;
						border-width: 1px;
						border-style: solid;
						border-top-color: #000000;
						border-bottom-color: #000000;
						border-left-color: #000000;
						border-right-color: #000000;
						background-color: #F3F3F3;
					}
				</style>
				<script type="text/javascript">
					function show_div(a)
					{
						if (a == ''){
							var divs = document.getElementsByTagName('div');
							for (var i = 0; i&lt;divs.length ;i++){
								divs[i].style.display = "";
							}
						}
						else {
							var divs = document.getElementsByTagName('div');
							for (var i = 0; i&lt;divs.length; i++){
								divs[i].style.display = "none";
							}
							var o = document.getElementById(a);
							o.style.display = "";
						}
					}
				</script>
			</head>
			<body>
				<table>
					<tbody>
						<tr>
							<td valign="top">
								<a href="javascript:void(0);" onclick="show_div('');">全部</a>
								<br/>
								<xsl:for-each select="./*">
									<a href="javascript:void(0);">
										<xsl:attribute name="onclick">
											show_div('<xsl:value-of select="name()"/>');
										</xsl:attribute>
										<xsl:value-of select="name()"/>
									</a>
									<br/>
								</xsl:for-each>
							</td>
							<td valign="top">
								<xsl:for-each select="./*">
									<div>
										<xsl:attribute name="id"><xsl:value-of select="name()"/></xsl:attribute>
										<table>
											<tbody>
												<tr>
													<td>name</td>
													<td>
														- <xsl:value-of select="name()"/>
													</td>
												</tr>
												<tr>
													<td>parent</td>
													<td>
														- <xsl:value-of select="@parent"/>
													</td>
												</tr>
												<tr>
													<td>notifies</td>
													<td>
														- <xsl:value-of select="@notifies"/>
													</td>
												</tr>
											</tbody>
										</table>
										<!--属性表-->
										<table border="1" class="style-table-1">
											<tbody>
												<tr>
													<td align="center" style="background-color:#6699CC; color:#FFFFFF">name</td>
													<td align="center" style="background-color:#6699CC; color:#FFFFFF">type</td>
													<td align="center" style="background-color:#6699CC; color:#FFFFFF">default</td>
													<td align="center" style="background-color:#6699CC; color:#FFFFFF">comment</td>
												</tr>
												<xsl:for-each select="./Attribute">
													<xsl:apply-templates select="."/>
												</xsl:for-each>
											</tbody>
										</table>
										<br/>
									</div>
								</xsl:for-each>
							</td>
						</tr>
					</tbody>
				</table>
			</body>
		</html>
	</xsl:template>
	<xsl:template match="Attribute">
		<tr>
			<td style="color:#DA0000; border-color:#000000; font-weight:bold;">
				<xsl:value-of select="@name"/>
			</td>
			<td>
				<xsl:value-of select="@type"/>
			</td>
			<td>
				<xsl:value-of select="@default"/>
			</td>
			<td>
				<xsl:value-of select="@comment"/>
			</td>
		</tr>
	</xsl:template>
</xsl:stylesheet>
