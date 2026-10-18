/*
 * Copyright (C) 2026 CyberWei922
 * SPDX-License-Identifier: Apache-2.0
 */
package org.evolution.ishtar.maintainer;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.ActivityNotFoundException;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.ContentValues;
import android.content.Intent;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.InputStream;
import java.io.OutputStream;

/** Local information page: no network, storage or privileged permission. */
public final class MaintainerActivity extends Activity {
    private LinearLayout content;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setTitle(R.string.app_name);
        if (getActionBar() != null) getActionBar().setDisplayHomeAsUpEnabled(true);
        ScrollView scroll = new ScrollView(this);
        content = new LinearLayout(this);
        content.setOrientation(LinearLayout.VERTICAL);
        int pad = dp(20);
        content.setPadding(pad, pad, pad, pad);
        scroll.addView(content);
        setContentView(scroll);
        text(R.string.maintainer_name, 28);
        text(R.string.build_description, 16);
        button(R.string.github_label, v -> openLink(R.string.github_url));
        text(R.string.community, 20);
        button(R.string.telegram, v -> openLink(R.string.telegram_url));
        button(R.string.qq_group, v -> {
            getSystemService(ClipboardManager.class).setPrimaryClip(
                    ClipData.newPlainText("QQ", getString(R.string.qq_number)));
            Toast.makeText(this, R.string.copied, Toast.LENGTH_SHORT).show();
        });
        button(R.string.support, v -> new AlertDialog.Builder(this)
                .setTitle(R.string.support)
                .setItems(new CharSequence[] {getString(R.string.wechat),
                        getString(R.string.alipay), getString(R.string.paypal)}, (d, which) -> {
                    if (which == 0) showCode(R.drawable.ishtar_donate_wechat,
                            R.string.wechat, "CyberWei-WeChat.jpg");
                    else if (which == 1) showCode(R.drawable.ishtar_donate_alipay,
                            R.string.alipay, "CyberWei-Alipay.jpg");
                    else openLink(R.string.paypal_url);
                }).setNegativeButton(android.R.string.cancel, null).show());
    }

    private int dp(int value) {
        return Math.round(value * getResources().getDisplayMetrics().density);
    }

    private void text(int resource, int size) {
        TextView view = new TextView(this);
        view.setText(resource);
        view.setTextSize(size);
        view.setPadding(0, dp(12), 0, dp(8));
        content.addView(view);
    }

    private void button(int resource, View.OnClickListener action) {
        Button button = new Button(this);
        button.setText(resource);
        button.setAllCaps(false);
        button.setOnClickListener(action);
        content.addView(button, new LinearLayout.LayoutParams(-1, -2));
    }

    private void openLink(int resource) {
        try {
            startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse(getString(resource))));
        } catch (ActivityNotFoundException exception) {
            Toast.makeText(this, R.string.no_browser, Toast.LENGTH_SHORT).show();
        }
    }

    private void showCode(int imageResource, int title, String filename) {
        LinearLayout body = new LinearLayout(this);
        body.setOrientation(LinearLayout.VERTICAL);
        body.setPadding(dp(12), dp(8), dp(12), dp(8));
        TextView help = new TextView(this);
        help.setText(R.string.qr_help);
        help.setPadding(0, 0, 0, dp(12));
        body.addView(help);
        ImageView image = new ImageView(this);
        image.setImageResource(imageResource);
        image.setContentDescription(getString(title));
        image.setAdjustViewBounds(true);
        image.setScaleType(ImageView.ScaleType.FIT_CENTER);
        image.setBackgroundColor(Color.WHITE);
        image.setForceDarkAllowed(false);
        body.addView(image, new LinearLayout.LayoutParams(-1, -2));
        ScrollView scroll = new ScrollView(this);
        scroll.addView(body);
        AlertDialog dialog = new AlertDialog.Builder(this).setTitle(title).setView(scroll)
                .setPositiveButton(R.string.save_image, null)
                .setNegativeButton(R.string.close, null).create();
        dialog.setOnShowListener(d -> dialog.getButton(AlertDialog.BUTTON_POSITIVE)
                .setOnClickListener(v -> {
                    dialog.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(false);
                    new Thread(() -> {
                        boolean saved = saveOriginal(imageResource, filename);
                        runOnUiThread(() -> {
                            Toast.makeText(this, saved ? R.string.image_saved : R.string.save_failed,
                                    Toast.LENGTH_LONG).show();
                            if (dialog.isShowing()) dialog.getButton(AlertDialog.BUTTON_POSITIVE)
                                    .setEnabled(true);
                        });
                    }, "SaveMaintainerImage").start();
                }));
        dialog.show();
    }

    private boolean saveOriginal(int resource, String filename) {
        Uri uri = null;
        try {
            ContentValues values = new ContentValues();
            values.put(MediaStore.Images.Media.DISPLAY_NAME, filename);
            values.put(MediaStore.Images.Media.MIME_TYPE, "image/jpeg");
            values.put(MediaStore.Images.Media.RELATIVE_PATH, "Pictures/Ishtar");
            values.put(MediaStore.Images.Media.IS_PENDING, 1);
            uri = getContentResolver().insert(MediaStore.Images.Media.EXTERNAL_CONTENT_URI, values);
            if (uri == null) return false;
            try (InputStream input = getResources().openRawResource(resource);
                    OutputStream output = getContentResolver().openOutputStream(uri)) {
                if (output == null) throw new java.io.IOException("No output stream");
                byte[] buffer = new byte[16384];
                int count;
                while ((count = input.read(buffer)) != -1) output.write(buffer, 0, count);
            }
            values.clear();
            values.put(MediaStore.Images.Media.IS_PENDING, 0);
            if (getContentResolver().update(uri, values, null, null) != 1)
                throw new java.io.IOException("Could not publish image");
            return true;
        } catch (Exception exception) {
            if (uri != null) {
                try { getContentResolver().delete(uri, null, null); }
                catch (RuntimeException ignored) { }
            }
            return false;
        }
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == android.R.id.home) {
            finish();
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
}
