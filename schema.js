const mongoose = require('mongoose');

const { Schema } = mongoose;

const status = ['REPORTED', 'MATCH_PENDING', 'MATCHED', 'RESOLVED'];

const historyEntrySchema = new Schema(
  {
    status: { type: String, enum: status, required: true },
    at: { type: Date, required: true, default: Date.now },
    note: { type: String, trim: true, maxlength: 500 }
  },
  { _id: false }
);

const itemSchema = new Schema(
  {
    id: { type: String, required: true, unique: true, immutable: true, trim: true },
    kind: { type: String, enum: ['LOST', 'FOUND'], required: true, immutable: true },

    category: { type: String, required: true, trim: true, minlength: 1, maxlength: 64 },
    month: { type: Number, required: true, min: 1, max: 12, validate: Number.isInteger },
    color: { type: String, required: true, trim: true, minlength: 1, maxlength: 40 },
    brand: { type: String, trim: true, maxlength: 80, default: null },
    description: { type: String, required: true, trim: true, minlength: 5, maxlength: 2000 },
    location: { type: String, required: true, trim: true, minlength: 1, maxlength: 200 },
    occurredAt: { type: Date, required: true },

    status: { type: String, enum: status, required: true, default: 'REPORTED' },
    history: { type: [historyEntrySchema], required: true, default: [] },

    reporterContact: { type: String, trim: true, minlength: 3, maxlength: 254 },
    finderContact: { type: String, trim: true, minlength: 3, maxlength: 254 },
    currentLocation: { type: String, trim: true, minlength: 1, maxlength: 200 }
  },
  {
    collection: 'items',
    timestamps: true,
    toJSON: { versionKey: false },
    toObject: { versionKey: false }
  }
);

itemSchema.pre('validate', function validateItemKind(next) {
  if (this.kind === 'LOST') {
    if (!this.reporterContact) this.invalidate('reporterContact', 'Required for LOST items.');
    if (this.finderContact != null) this.invalidate('finderContact', 'Only valid for FOUND items.');
    if (this.currentLocation != null) {
      this.invalidate('currentLocation', 'Only valid for FOUND items.');
    }
  } else if (this.kind === 'FOUND') {
    if (!this.finderContact) this.invalidate('finderContact', 'Required for FOUND items.');
    if (!this.currentLocation) {
      this.invalidate('currentLocation', 'Required for FOUND items.');
    }
    if (this.reporterContact != null) this.invalidate('reporterContact', 'Only valid for LOST items.');
  }
  next();
});

module.exports = mongoose.model('Item', itemSchema);
