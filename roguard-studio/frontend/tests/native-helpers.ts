import {expect,type Page} from "@playwright/test";

export async function nativeAction(page:Page, action:"start"|"restart"="start") {
  const dashboard=process.env.ROGUARD_TEST_DASHBOARD;
  if(!dashboard)throw new Error("Native browser tests require ROGUARD_TEST_DASHBOARD pointing to the dedicated studio_native_services.py test dashboard.");
  await page.getByRole("button",{name:action==="start"?"Start LTE network":"Restart chain",exact:true}).click();
  const dialog=page.getByRole("dialog",{name:"Connect RoGuard before simulation"});
  await expect(dialog).toBeVisible();
  await dialog.getByLabel("RoGuard dashboard URL").fill(dashboard);
  await dialog.getByRole("button",{name:action==="start"?"Connect & start simulation":"Connect & restart simulation",exact:true}).click();
  await expect(dialog).toBeHidden({timeout:40000});
}
